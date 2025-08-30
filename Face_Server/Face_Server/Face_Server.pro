QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 添加OpenCV和SeetaFace的头文件路径
INCLUDEPATH += "E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/OpenCV_install/include"
INCLUDEPATH += "E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/OpenCV_install/include/opencv2"
INCLUDEPATH += "E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/SeetaFace_install/include/seeta"
INCLUDEPATH += "E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/SeetaFace_install/include"

# 添加OpenCV和SeetaFace的库文件路径
LIBS += "E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/OpenCV_install/x64/mingw/lib/libopencv*"
LIBS += "E:/Program/Linux/Linux_Projects/ARM-DoorLock-Project/SeetaFace_install/lib/lib*"

SOURCES += \
    main.cpp \
    face_serverwin.cpp \
    qfaceobject.cpp \
    registerwin.cpp \
    selectwin.cpp

HEADERS += \
    face_serverwin.h \
    qfaceobject.h \
    registerwin.h \
    selectwin.h

FORMS += \
    face_serverwin.ui \
    registerwin.ui \
    selectwin.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
