#include "face_serverwin.h"
#include "ui_face_serverwin.h"
#include <QDateTime>
#include <QThread>
#include <opencv.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include "../../proto/protobuf_helper.h"

using namespace door_lock;

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

    // 尝试解析Protobuf消息（精简版）
    ImageMessage imageMsg;
    if (ProtobufHelper::deserializeImageMessage(data, imageMsg)) {
        // 使用Protobuf协议
        std::string imageDataStr = imageMsg.image_data();
        QByteArray imageBytes(imageDataStr.c_str(), imageDataStr.size());
        
        // 在UI显示图像
        QPixmap mmp;
        mmp.loadFromData(imageBytes, "jpg");
        mmp = mmp.scaled(ui->picture->size());
        ui->picture->setPixmap(mmp);
        
        // 转换为OpenCV Mat进行识别
        std::vector<uchar> decode(imageBytes.begin(), imageBytes.end());
        cv::Mat faceImage = cv::imdecode(decode, cv::IMREAD_COLOR);
        
        qDebug() << "Received protobuf image data, size:" << imageBytes.size();
        
        // 触发人脸识别
        emit query(faceImage);
    } else {
        // 使用原有的图像数据协议（向后兼容）
        QPixmap mmp;
        mmp.loadFromData(data,"jpg");
        mmp = mmp.scaled(ui->picture->size());
        ui->picture->setPixmap(mmp);

        //识别人脸
        cv::Mat faceImage;
        std::vector<uchar> decode;
        decode.resize(data.size());
        memcpy(decode.data(),data.data(),data.size());
        faceImage = cv::imdecode(decode, cv::IMREAD_COLOR);
        
        qDebug() << "Received legacy image data, size:" << data.size();
        emit query(faceImage);
    }
}

void Face_ServerWin::recv_faceid(int64_t faceid)
{
    qDebug()<<"识别到的人脸id:"<<faceid;
    
    if(faceid < 0) {
        // 识别失败 - 发送Protobuf响应（精简版）
        ResultMessage result = ProtobufHelper::createResultMessage(false);
        QByteArray responseData = ProtobufHelper::serializeResultMessage(result);
        msocket->write(responseData);
        
        // 向后兼容 - 同时发送JSON响应
        QString sdmsg = QString("{\"personnelID\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}");
        msocket->write(sdmsg.toUtf8());
        return;
    }
    
    // 从数据库中查询faceid对应的个人信息
    model.setFilter(QString("faceID=%1").arg(faceid));
    model.select();
    
    if(model.rowCount() == 1) {
        QSqlRecord record = model.record(0);
        
        // 记录考勤
        QString insertSql = QString("insert into attendance(personnelID, name) values('%1', '%2')")
                            .arg(record.value("personnelID").toString())
                            .arg(record.value("name").toString());
        QSqlQuery query;
        
        if(!query.exec(insertSql)) {
            // 数据库错误 - 发送失败响应
            ResultMessage result = ProtobufHelper::createResultMessage(false);
            QByteArray responseData = ProtobufHelper::serializeResultMessage(result);
            msocket->write(responseData);
            
            // 向后兼容
            QString sdmsg = QString("{\"personnelID\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}");
            msocket->write(sdmsg.toUtf8());
            return;
        } else {
            // 识别成功 - 发送Protobuf响应（精简版）
            ResultMessage result = ProtobufHelper::createResultMessage(
                true,
                record.value("personnelID").toString(),
                record.value("name").toString(),
                "软件",
                QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            );
            QByteArray responseData = ProtobufHelper::serializeResultMessage(result);
            msocket->write(responseData);
            
            // 向后兼容 - 同时发送JSON响应
            QString successMsg = QString("{\"personnelID\":\"%1\",\"name\":\"%2\",\"department\":\"软件\",\"time\":\"%3\"}")
                .arg(record.value("personnelID").toString())
                .arg(record.value("name").toString())
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            msocket->write(successMsg.toUtf8());
            
            qDebug() << "Sent protobuf + JSON response, protobuf size:" << responseData.size();
        }
    }
}
