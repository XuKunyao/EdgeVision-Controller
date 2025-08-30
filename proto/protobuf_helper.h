#ifndef PROTOBUF_HELPER_H
#define PROTOBUF_HELPER_H

#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <opencv.hpp>
#include "../proto/door_lock_messages.pb.h"

using namespace door_lock;

class ProtobufHelper
{
public:
    // 创建图像数据消息
    static DoorLockMessage createImageMessage(const cv::Mat& image, const QString& deviceId = "door_001");
    
    // 创建识别结果消息
    static DoorLockMessage createRecognitionResult(
        RecognitionResult::Status status,
        const QString& personnelId = "",
        const QString& name = "",
        const QString& department = "",
        float confidence = 0.0f,
        int64_t faceId = -1
    );
    
    // 创建控制命令消息
    static DoorLockMessage createControlCommand(
        ControlCommand::CommandType type,
        const QString& payload = ""
    );
    
    // 序列化消息到QByteArray
    static QByteArray serializeMessage(const DoorLockMessage& message);
    
    // 从QByteArray反序列化消息
    static bool deserializeMessage(const QByteArray& data, DoorLockMessage& message);
    
    // 辅助函数：cv::Mat转换为字节数组
    static QByteArray matToByteArray(const cv::Mat& mat, const QString& format = "jpg");
    
    // 辅助函数：字节数组转换为cv::Mat
    static cv::Mat byteArrayToMat(const QByteArray& data);
    
private:
    ProtobufHelper() = default;
};

#endif // PROTOBUF_HELPER_H
