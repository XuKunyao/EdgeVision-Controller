#ifndef FACE_DOORLOCK_H
#define FACE_DOORLOCK_H

#include <QMainWindow>
#include <opencv2/opencv.hpp> //包含opencv头文件
#include <QTcpSocket>
#include <QTimer>

using namespace cv;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class Face_DoorLock; }
QT_END_NAMESPACE

class Face_DoorLock : public QMainWindow
{
    Q_OBJECT

public:
    Face_DoorLock(QWidget *parent = nullptr);
    ~Face_DoorLock();
    //定时器事件
    void timerEvent(QTimerEvent *e);
private slots:
    void timer_connect();
    void stop_connect();
    void start_connect();
    void recv_data();
private:
    Ui::Face_DoorLock *ui;

    //摄像头
    VideoCapture cap;
    //haar--级联分类器
    cv::CascadeClassifier cascade;

    //创建网络套接字，定时器
    QTcpSocket msocket;
    QTimer mtimer;

    //标志是否是同一个人脸进入到识别区域
    int flag;

    //保存人脸的数据
    cv::Mat faceMat;
};
#endif // FACE_DOORLOCK_H
