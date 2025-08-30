#include "face_serverwin.h"

#include <QApplication>
#include <QSqlDatabase> // Qt SQL数据库模块头文件
#include <QSqlError> // Qt SQL错误处理头文件
#include <QSqlQuery> // Qt SQL查询操作头文件

#include "registerwin.h"
#include "selectwin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<cv::Mat>("cv::Mat&");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<int64_t>("int64_t");

    /******************** 数据库初始化部分 ​********************/
    // 添加SQLite数据库驱动（QSQLITE是Qt内置的SQLite驱动标识符）
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // 设置数据库文件名称（若文件不存在会自动创建）
    // 文件路径为当前程序运行目录下的server.db
    db.setDatabaseName("server.db");
    // 尝试打开数据库连接
    if(!db.open())
    {
        // 输出详细的数据库错误信息到调试控制台
        qDebug()<<db.lastError().text();
        return -1;
    }

    /******************** 创建人员信息表 ​********************/
    // 构建创建人员表的SQL语句（使用跨行字符串拼接）
    QString createsql =
            "create table if not exists personnel("
            "personnelID integer primary key autoincrement,"
            "name varchar(256), "
            "sex varchar(32),"
            "birthday text, "
            "address text, "
            "phone text, "
            "faceID integer unique, "
            "headfile text"
            ")";

    // 创建用于执行SQL语句的查询对象
    QSqlQuery query;

    // 执行建表语句（注意：SQLite执行失败通常是因为语法错误或权限问题）
    if(!query.exec(createsql))
    {
       qDebug()<<query.lastError().text();
       return -1;
    }

    /******************** 创建登记记录表 ​********************/
    createsql = "create table if not exists attendance("
                "attendanceID integer primary key autoincrement, "
                "personnelID integer,"
                "name varchar(256), "
                "attendanceTime TimeStamp NOT NULL DEFAULT(datetime('now','localtime'))"
                ")";
    // 再次使用同一个query对象执行SQL（需确保前一个查询已完成）
    if(!query.exec(createsql))
    {
       qDebug()<<query.lastError().text();
       return -1;
    }


    Face_ServerWin w;// 人脸识别显示界面
    w.show();

    // RegisterWin ww;// 人员注册界面
    // ww.show();

    // SelectWin sw; // 登录信息和人员信息查询界面
    // sw.show();

    return a.exec();
}
