#include "protobuf_helper.h"
#include <vector>
#include <QDebug>

DoorLockMessage ProtobufHelper::createImageMessage(const cv::Mat& image, const QString& deviceId)
{
    DoorLockMessage message;
    message.set_type(DoorLockMessage::IMAGE_DATA);
    
    ImageData* imageData = message.mutable_image_data();
    
    // 将cv::Mat编码为JPG格式
    QByteArray imageBytes = matToByteArray(image, "jpg");
    
    imageData->set_image_data(imageBytes.data(), imageBytes.size());
    imageData->set_format("jpg");
    imageData->set_timestamp(QDateTime::currentMSecsSinceEpoch());
    imageData->set_device_id(deviceId.toStdString());
    imageData->set_image_width(image.cols);
    imageData->set_image_height(image.rows);
    
    return message;
}

DoorLockMessage ProtobufHelper::createRecognitionResult(
    RecognitionResult::Status status,
    const QString& personnelId,
    const QString& name,
    const QString& department,
    float confidence,
    int64_t faceId)
{
    DoorLockMessage message;
    message.set_type(DoorLockMessage::RECOGNITION_RESULT);
    
    RecognitionResult* result = message.mutable_recognition_result();
    result->set_status(status);
    result->set_timestamp(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString());
    result->set_confidence(confidence);
    result->set_face_id(faceId);
    
    if (status == RecognitionResult::SUCCESS) {
        PersonInfo* person = result->mutable_person();
        person->set_personnel_id(personnelId.toStdString());
        person->set_name(name.toStdString());
        person->set_department(department.toStdString());
        person->set_face_id(faceId);
        
        result->set_message("识别成功");
    } else {
        switch (status) {
        case RecognitionResult::FAILED:
            result->set_message("识别失败：未找到匹配人员");
            break;
        case RecognitionResult::NO_FACE:
            result->set_message("未检测到人脸");
            break;
        case RecognitionResult::LOW_QUALITY:
            result->set_message("图像质量过低");
            break;
        case RecognitionResult::DATABASE_ERROR:
            result->set_message("数据库错误");
            break;
        default:
            result->set_message("未知错误");
        }
    }
    
    return message;
}

DoorLockMessage ProtobufHelper::createControlCommand(ControlCommand::CommandType type, const QString& payload)
{
    DoorLockMessage message;
    message.set_type(DoorLockMessage::CONTROL_COMMAND);
    
    ControlCommand* command = message.mutable_control_command();
    command->set_type(type);
    command->set_payload(payload.toStdString());
    command->set_timestamp(QDateTime::currentMSecsSinceEpoch());
    
    return message;
}

QByteArray ProtobufHelper::serializeMessage(const DoorLockMessage& message)
{
    std::string serialized = message.SerializeAsString();
    return QByteArray(serialized.data(), serialized.size());
}

bool ProtobufHelper::deserializeMessage(const QByteArray& data, DoorLockMessage& message)
{
    return message.ParseFromArray(data.data(), data.size());
}

QByteArray ProtobufHelper::matToByteArray(const cv::Mat& mat, const QString& format)
{
    std::vector<uchar> buffer;
    std::string ext = "." + format.toStdString();
    
    if (cv::imencode(ext, mat, buffer)) {
        return QByteArray(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    }
    
    qDebug() << "Failed to encode image to" << format;
    return QByteArray();
}

cv::Mat ProtobufHelper::byteArrayToMat(const QByteArray& data)
{
    std::vector<uchar> buffer(data.begin(), data.end());
    return cv::imdecode(buffer, cv::IMREAD_COLOR);
}
