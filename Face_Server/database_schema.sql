-- ==========================================
-- 智能门锁系统数据库表结构
-- 创建时间: 2025-08-30
-- 说明: 包含人员信息表和考勤记录表
-- ==========================================

-- 删除已存在的表（开发阶段使用，生产环境请谨慎）
-- DROP TABLE IF EXISTS attendance;
-- DROP TABLE IF EXISTS personnel;

-- 人员信息表
CREATE TABLE IF NOT EXISTS personnel (
    personnelID INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    sex TEXT CHECK(sex IN ('男', '女', '其他')),
    birthday DATE,
    address TEXT,
    phone TEXT,
    faceID INTEGER UNIQUE,
    headfile TEXT,
    department TEXT DEFAULT '未分配',
    status TEXT DEFAULT '正常' CHECK(status IN ('正常', '禁用', '离职')),
    created_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 考勤记录表
CREATE TABLE IF NOT EXISTS attendance (
    attendanceID INTEGER PRIMARY KEY AUTOINCREMENT,
    personnelID INTEGER NOT NULL,
    name TEXT NOT NULL,
    attendanceTime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    access_type TEXT DEFAULT '人脸识别' CHECK(access_type IN ('人脸识别', '刷卡', '密码', '管理员')),
    device_id TEXT,
    FOREIGN KEY (personnelID) REFERENCES personnel(personnelID)
);

-- 创建索引提高查询性能
CREATE INDEX IF NOT EXISTS idx_personnel_faceid ON personnel(faceID);
CREATE INDEX IF NOT EXISTS idx_attendance_personnel ON attendance(personnelID);
CREATE INDEX IF NOT EXISTS idx_attendance_time ON attendance(attendanceTime);

-- 插入示例数据（可选）
INSERT OR IGNORE INTO personnel (name, sex, department, faceID) VALUES 
('张三', '男', '技术部', 1001),
('李四', '女', '人事部', 1002),
('王五', '男', '财务部', 1003);

-- 数据库版本信息表（用于后续升级管理）
CREATE TABLE IF NOT EXISTS db_version (
    version TEXT PRIMARY KEY,
    applied_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

INSERT OR REPLACE INTO db_version (version) VALUES ('1.0.0');
