#ifndef PROTOBUF_HELPER_H
#define PROTOBUF_HELPER_H

#include <QByteArray>
#include <QDataStream>
#include <opencv2/opencv.hpp>
#include "door_lock_messages.pb.h"

using namespace door_lock;

class ProtobufHelper
{
public:
    // 创建图像消息
    static ImageMessage createImageMessage(const cv::Mat& image);
    
    // 创建识别结果消息
    static ResultMessage createResultMessage(bool success, 
                                           const QString& personnelId = "",
                                           const QString& name = "",
                                           const QString& department = "",
                                           const QString& timestamp = "");
    
    // 序列化消息
    static QByteArray serializeImageMessage(const ImageMessage& message);
    static QByteArray serializeResultMessage(const ResultMessage& message);
    
    // 反序列化消息
    static bool deserializeImageMessage(const QByteArray& data, ImageMessage& message);
    static bool deserializeResultMessage(const QByteArray& data, ResultMessage& message);
};

#endif // PROTOBUF_HELPER_H
