QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

RESOURCES += \
    res/res.qrc

RC_ICONS = logo.ico # 配置桌面软件和的图标

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(3rdparty/3rdparty.pri)
include(Index/Index.pri)
include(Record/Record.pri)
include(VNCServer/VNCServer.pri)
include(VNCClient/VNCClient.pri)
include(Utils/Utils.pri)

win32: LIBS += -ld3d11
win32: LIBS += -L$$PWD/3rdparty/ffmpeg/lib/ -lavcodec -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale

INCLUDEPATH += $$PWD/3rdparty/ffmpeg/include
DEPENDPATH += $$PWD/3rdparty/ffmpeg/include
