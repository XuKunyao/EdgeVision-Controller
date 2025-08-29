#include "face_serverwin.h"
#include "ui_face_serverwin.h"
#include <QDateTime>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <QSqlQuery>
#include <QSqlRecord>

Face_ServerWin::Face_ServerWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Face_ServerWin)
{
    ui->setupUi(this);
    //qtcpServer当有客户端连会发送newconnection
    connect(&mserver, &QTcpServer::newConnection, this, &Face_ServerWin::accept_client);
    // 启动服务器监听，监听所有网络接口的9999端口
    mserver.listen(QHostAddress::Any,9999);
    // 初始化数据包大小为0（用于记录当前接收数据包长度）
    bsize = 0;

    //给sql模型绑定表格
    model.setTable("personnel");

    //创建一个线程
    QThread *thread = new QThread();
    //把QFaceObject对象移动到thread线程中执行
    fobj.moveToThread(thread);
    //启动线程，此时启动线程还不会执行
    thread->start();
    connect(this,&Face_ServerWin::query,&fobj,&QFaceObject::face_query);
    //关联QFaceObject对象里面的send_faceid信号
    connect(&fobj,&QFaceObject::send_faceid,this, &Face_ServerWin::recv_faceid);
}

Face_ServerWin::~Face_ServerWin()
{
    delete ui;
}

//接受客户端连接
void Face_ServerWin::accept_client()
{
    //获取与客户端通信的套接字
    msocket = mserver.nextPendingConnection();

    //当客户端有数据到达会发送readyRead信号
    connect(msocket,&QTcpSocket::readyRead,this,&Face_ServerWin::read_data);
}

//读取客户端发送的数据
void Face_ServerWin::read_data()
{
    //读取所有的数据
    //QString msg = msocket->readAll();
    //qDebug()<<msg;

    // 创建数据流对象并与套接字绑定
    QDataStream stream(msocket);
    // 设置数据流版本（需与客户端一致）
    stream.setVersion(QDataStream::Qt_5_14);

    // 如果当前数据包大小为0，表示需要读取新数据包头
    if(bsize == 0){
        // 检查可读数据是否足够获取包头大小(sizeof(bsize))
        if(msocket->bytesAvailable()<(qint64)sizeof(bsize)) return ;
        // 从数据流中读取数据包长度到bsize变量
        stream>>bsize;

    }

    // 检查剩余数据是否足够当前数据包大小
    if(msocket->bytesAvailable() < bsize)//说明数据还没有发送完成，返回继续等待
    {
        return ;
    }

    // 创建字节数组存储实际数据
    QByteArray data;
    // 从数据流中提取数据到字节数组
    stream>>data;
    // 重置数据包大小，准备接收下一个包
    bsize = 0;
    // 检查数据有效性
    if(data.size() == 0)//没有读取到数据
    {
        return;
    }

    // 将字节数据转换为图片对象
    QPixmap mmp;
    // 从二进制数据加载jpg格式图片
    mmp.loadFromData(data,"jpg");
    // 缩放图片到picture标签的大小
    mmp = mmp.scaled(ui->picture->size());
    // 在UI的picture标签上显示图片
    ui->picture->setPixmap(mmp);

    //识别人脸
    cv::Mat faceImage; // 创建OpenCV的Mat对象，用于存储解码后的图像数据
    std::vector<uchar> decode; // 创建一个存储原始图像字节流的vector容器
    // 调整vector容器大小为接收到的数据大小，预分配内存空间
    // data.size()：获取QByteArray数据的总字节数
    decode.resize(data.size());
    // 将QByteArray中的图像字节流复制到vector容器中
    // decode.data()：获取vector容器底层的字节数组指针（类型为uchar*）
    // data.data()：获取QByteArray底层的数据指针（类型为const char*）
    // data.size()：复制的字节数
    memcpy(decode.data(),data.data(),data.size());
    // 使用OpenCV解码内存中的图像数据
    // decode：包含JPEG/PNG等压缩格式的字节流
    // cv::IMREAD_COLOR：以BGR三通道格式加载图像（默认格式，适合人脸识别）
    faceImage = cv::imdecode(decode, cv::IMREAD_COLOR);
    // 调用人脸识别模块的查询接口
    // faceImage：解码后的图像数据
    // faceid：返回识别到的人脸ID（int64_t类型，未识别到返回-1）
    // int faceid = fobj.face_query(faceImage); //太消耗资源了
    emit query(faceImage);
}

void Face_ServerWin::recv_faceid(int64_t faceid)
{
    qDebug()<<"识别到的人脸id:"<<faceid;
    // 查询失败
    if(faceid < 0)
    {
        QString sdmsg = QString("{\"personnelID\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}");
        msocket->write(sdmsg.toUtf8());//把打包好的数据发送给客户端
        return ;
    }
    // 从数据库中查询faceid对应的个人信息
    model.setFilter(QString("faceID=%1").arg(faceid));
    //查询
    model.select();
    //判断是否查询到数据
    if(model.rowCount() == 1) // 检查数据库查询结果是否为1条记录
    {
        // 注释说明要构建的JSON字段：工号、姓名、部门、时间
        //{personnelID:%1,name:%2,department:软件,time:%3}
        QSqlRecord record = model.record(0); // 获取查询结果的第一行记录
        QString sdmsg = QString("{\"personnelID\":\"%1\",\"name\":\"%2\",\"department\":\"软件\",\"time\":\"%3\"}")
                .arg(record.value("personnelID").toString())
                .arg(record.value("name").toString())
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

        //把数据写入数据库--登记表
        QString insertSql = QString("insert into attendance(personnelID, name) values('%1', '%2')")
                            .arg(record.value("personnelID").toString())
                            .arg(record.value("name").toString());
        QSqlQuery query;
        if(!query.exec(insertSql))// 如果插入失败则构造一个空数据的JSON响应
        {
            QString sdmsg = QString("{\"personnelID\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}");
            msocket->write(sdmsg.toUtf8());//把打包好的数据发送给客户端
            return ;
        }else
        {
            // 构造成功响应的JSON（从数据库记录中获取值）
            QSqlRecord record = model.record(0);
            QString successMsg = QString("{\"personnelID\":\"%1\",\"name\":\"%2\",\"department\":\"软件\",\"time\":\"%3\"}")
                .arg(record.value("personnelID").toString())
                .arg(record.value("name").toString())
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

            // 发送成功数据到客户端
            msocket->write(successMsg.toUtf8());
        }
    }
}
