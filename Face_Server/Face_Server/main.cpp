#include "face_serverwin.h"

#include <QApplication>
#include <QSqlDatabase> // Qt SQL数据库模块头文件
#include <QSqlError> // Qt SQL错误处理头文件
#include <QSqlQuery> // Qt SQL查询操作头文件

#include "registerwin.h"
#include "selectwin.h"
#include "databasemanager.h"  // 新增：包含数据库管理器

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<cv::Mat>("cv::Mat&");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<int64_t>("int64_t");

    /******************** 使用新的数据库管理器 ********************/
    DatabaseManager dbManager;
    
    // 简单的一行代码完成所有数据库初始化工作
    if (!dbManager.initializeDatabase("server.db")) {
        qDebug() << "数据库初始化失败";
        return -1;
    }
    
    qDebug() << "数据库版本:" << dbManager.getDatabaseVersion();


    Face_ServerWin w;// 人脸识别显示界面
    w.show();

    // RegisterWin ww;// 人员注册界面
    // ww.show();

    // SelectWin sw; // 登录信息和人员信息查询界面
    // sw.show();

    return a.exec();
}
