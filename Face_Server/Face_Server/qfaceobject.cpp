#include "qfaceobject.h"
#include <QDebug>

QFaceObject::QFaceObject(QObject *parent) : QObject(parent)
{
#ifndef DISABLE_SEETAFACE
    // 初始化 SeetaFace 模型路径和计算设备
    // 加载人脸检测（FD）、关键点检测（PD）和特征提取（FR）模型
    seeta::ModelSetting  FDmode("E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/SeetaFace_install/bin/model/fd_2_00.dat",seeta::ModelSetting::CPU,0);
    seeta::ModelSetting  PDmode("E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/SeetaFace_install/bin/model/pd_2_00_pts5.dat",seeta::ModelSetting::CPU,0);
    seeta::ModelSetting  FRmode("E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/SeetaFace_install/bin/model/fr_2_10.dat",seeta::ModelSetting::CPU,0);
    // 创建 SeetaFace 引擎对象
    this->fengineptr = new seeta::FaceEngine(FDmode,PDmode,FRmode);

    // 导入已有的人脸数据库
    this->fengineptr->Load("./face.db");
#else
    qDebug() << "SeetaFace is disabled for this build. Face recognition features are not available.";
#endif
}

QFaceObject::~QFaceObject()
{
#ifndef DISABLE_SEETAFACE
    delete fengineptr;
#endif
}

int64_t QFaceObject::face_register(cv::Mat &faceImage)
{
#ifndef DISABLE_SEETAFACE
    //把opencv的Mat数据转为seetaface的数据
    SeetaImageData simage;
    simage.data = faceImage.data;
    simage.width = faceImage.cols;
    simage.height = faceImage.rows;
    simage.channels = faceImage.channels();

    // 调用引擎注册人脸，返回人脸 ID
    int64_t faceid = this->fengineptr->Register(simage);//注册返回一个人脸id

    // 注册成功则保存到数据库
    if(faceid>=0){
        fengineptr->Save("./face.db");
    }
    return faceid;
#else
    Q_UNUSED(faceImage)
    qDebug() << "Face registration is disabled (SeetaFace not available)";
    return -1;
#endif
}

int QFaceObject::face_query(cv::Mat &faceImage)
{
#ifndef DISABLE_SEETAFACE
    //把opencv的Mat数据转为seetaface的数据
    SeetaImageData simage;
    simage.data = faceImage.data;
    simage.width = faceImage.cols;
    simage.height = faceImage.rows;
    simage.channels = faceImage.channels();

    // 调用引擎查询人脸，返回匹配的 ID 和相似度
    float similarity=0;
    int64_t faceid = fengineptr->Query(simage,&similarity);//运行时间比较长
    qDebug()<<"人脸ID:"<<faceid<<"相似度:"<<similarity;
    // 添加一个条件判断，相识度大于0.7才判定
    if(similarity > 0.7)
    {
        emit send_faceid(faceid); // 有了这句发送信号后，下面的return faceid其实可以省略了
    }else
    {
        emit send_faceid(-1);
    }
    return faceid;
#else
    Q_UNUSED(faceImage)
    qDebug() << "Face query is disabled (SeetaFace not available)";
    emit send_faceid(-1);
    return -1;
#endif
}


