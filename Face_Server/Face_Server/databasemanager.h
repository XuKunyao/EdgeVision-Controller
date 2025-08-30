#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QTextStream>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    
    // 初始化数据库
    bool initializeDatabase(const QString &dbPath = "server.db");
    
    // 从SQL文件执行脚本
    bool executeScriptFromFile(const QString &scriptPath);
    
    // 检查数据库版本
    QString getDatabaseVersion();
    
    // 升级数据库
    bool upgradeDatabase(const QString &targetVersion);
    
private:
    QSqlDatabase db;
    
    // 执行单条SQL语句
    bool executeSql(const QString &sql);
    
    // 创建基础表结构
    bool createBasicTables();
};

#endif // DATABASEMANAGER_H
