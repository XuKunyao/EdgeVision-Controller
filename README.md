# EdgeVision-Controller
基于韦东山Imx6ull-pro开发板和OpenCV4.5.2和SeetaFace2
**技术栈：Linux设备驱动、Platform总线、TCP数据(socket)、OpenCV使用、人脸识别算法应用、多线程、QT设计**

- 利用**QT设计**客户端与服务端可视化**UI界面**；在PC端与Linux端对OpenCV和人脸算法模型进行编译并配置；

- Linux环境下**交叉编译**客户端程序，移植于**ARM：**将图像进行**编码与解码**，利用TCP完成客户端与服务器端**图像数据传输**；
- 导入人脸算法模型，实现服务端**人脸信息注册**，**开辟多线程分别进行人脸图像接收、识别、信息发送**，优化服务器端工作速率；
- 将识别信息转换为JSON数据发送，**创建数据库**，将人脸注册信息与门锁进入的信息进行存储与管理；
  完成门禁开关的**设备驱动程序**；利用Platform总线进行设备/驱动分离**，修改设备树**完成驱动注册
- 交叉编译OpenCV库，与人脸算法模型移植到ARM端，在**应用层**实现**驱动调用**，完成硬件控制。

Face_DoorLock为客户端程序，Face_Server为服务器端程序
opencv_contrib-4.5.2和opencv-4.5.2为所需的OpenCV文件资源，SeetaFace2为所需的人脸识别文件资源
OpenCV_install和SeetaFace_install分别为OpenCV和Seetaface编译之后得到的文件
