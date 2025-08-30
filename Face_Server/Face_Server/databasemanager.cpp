#include "databasemanager.h"

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
}

bool DatabaseManager::initializeDatabase(const QString &dbPath)
{
    // 添加SQLite数据库驱动
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return false;
    }
    
    qDebug() << "数据库连接成功:" << dbPath;
    
    // 尝试从SQL文件创建表结构
    if (QFile::exists("../database_schema.sql")) {
        return executeScriptFromFile("../database_schema.sql");
    } else {
        // 如果SQL文件不存在，使用代码创建基础表
        qDebug() << "SQL脚本文件不存在，使用代码创建基础表结构";
        return createBasicTables();
    }
}

bool DatabaseManager::executeScriptFromFile(const QString &scriptPath)
{
    QFile file(scriptPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开SQL脚本文件:" << scriptPath;
        return false;
    }
    
    QTextStream in(&file);
    QString sqlScript = in.readAll();
    file.close();
    
    // 按分号分割SQL语句
    QStringList sqlStatements = sqlScript.split(';', Qt::SkipEmptyParts);
    
    QSqlQuery query(db);
    for (const QString &sql : sqlStatements) {
        QString trimmedSql = sql.trimmed();
        if (trimmedSql.isEmpty() || trimmedSql.startsWith("--")) {
            continue; // 跳过空行和注释
        }
        
        if (!query.exec(trimmedSql)) {
            qDebug() << "SQL执行失败:" << trimmedSql;
            qDebug() << "错误信息:" << query.lastError().text();
            return false;
        }
    }
    
    qDebug() << "数据库表结构创建成功";
    return true;
}

bool DatabaseManager::executeSql(const QString &sql)
{
    QSqlQuery query(db);
    if (!query.exec(sql)) {
        qDebug() << "SQL执行失败:" << sql;
        qDebug() << "错误信息:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::createBasicTables()
{
    // 创建人员信息表
    QString createPersonnelTable = R"(
        CREATE TABLE IF NOT EXISTS personnel (
            personnelID INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            sex TEXT,
            birthday DATE,
            address TEXT,
            phone TEXT,
            faceID INTEGER UNIQUE,
            headfile TEXT,
            department TEXT DEFAULT '未分配',
            status TEXT DEFAULT '正常',
            created_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    // 创建考勤记录表
    QString createAttendanceTable = R"(
        CREATE TABLE IF NOT EXISTS attendance (
            attendanceID INTEGER PRIMARY KEY AUTOINCREMENT,
            personnelID INTEGER NOT NULL,
            name TEXT NOT NULL,
            attendanceTime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            access_type TEXT DEFAULT '人脸识别',
            device_id TEXT
        )
    )";
    
    // 创建版本表
    QString createVersionTable = R"(
        CREATE TABLE IF NOT EXISTS db_version (
            version TEXT PRIMARY KEY,
            applied_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    return executeSql(createPersonnelTable) && 
           executeSql(createAttendanceTable) && 
           executeSql(createVersionTable) &&
           executeSql("INSERT OR REPLACE INTO db_version (version) VALUES ('1.0.0')");
}

QString DatabaseManager::getDatabaseVersion()
{
    QSqlQuery query("SELECT version FROM db_version ORDER BY applied_time DESC LIMIT 1", db);
    if (query.next()) {
        return query.value(0).toString();
    }
    return "未知版本";
}

bool DatabaseManager::upgradeDatabase(const QString &targetVersion)
{
    QString currentVersion = getDatabaseVersion();
    qDebug() << "当前数据库版本:" << currentVersion << "目标版本:" << targetVersion;
    
    // 这里可以添加版本升级逻辑
    // 例如：从1.0.0升级到1.1.0时需要执行的SQL
    
    return true;
}
