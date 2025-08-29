#ifndef REGISTERWIN_H
#define REGISTERWIN_H

#include <QWidget>
#include <opencv2/opencv.hpp> //包含opencv头文件

namespace Ui {
class RegisterWin;
}

class RegisterWin : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWin(QWidget *parent = nullptr);
    ~RegisterWin();
    void timerEvent(QTimerEvent *e);

private slots:
    void on_reset_clicked();

    void on_addpic_clicked();

    void on_Register_clicked();

    void on_camera_clicked();

    void on_takephoto_clicked();

private:
    Ui::RegisterWin *ui;
    int timerid;
    //摄像头
    cv::VideoCapture cap;
    cv::Mat image;
};

#endif // REGISTERWIN_H
