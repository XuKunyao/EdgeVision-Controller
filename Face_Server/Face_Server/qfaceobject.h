#ifndef QFACEOBJECT_H
#define QFACEOBJECT_H

#include <QObject>
#ifndef DISABLE_SEETAFACE
#include <seeta/FaceEngine.h>
#endif
#include <opencv2/opencv.hpp>

//人脸数据存储， 人脸检测，人脸识别
class QFaceObject : public QObject
{
    Q_OBJECT
public:
    explicit QFaceObject(QObject *parent = nullptr);
    ~QFaceObject();
public slots:
    // 注册人脸：输入 OpenCV 图像，返回人脸 ID（int64_t）
    int64_t face_register(cv::Mat& faceImage);
    // 查询人脸：输入 OpenCV 图像，返回匹配的人脸 ID（int）
    int  face_query(cv::Mat& faceImage);
signals:
    void send_faceid(int64_t faceid);
private:
#ifndef DISABLE_SEETAFACE
    // SeetaFace 引擎指针
    seeta::FaceEngine  *fengineptr;
#endif
};

#endif // QFACEOBJECT_H
