#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QScreen>
#include "SingletonUtils.h"
#include "IndexWidget.h"
#include <QDebug>
#include <QsLog.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_StyledBackground,true);
    setStyleSheet(QString(".MainWindow{background-color:%1;}").arg("rgb(31,33,42)"));
    QString title = QString("SRE V%1 (64-bit,windows)").arg(QCoreApplication::applicationVersion());
    setWindowTitle(title);

    QList<QScreen *> screens = QGuiApplication::screens();//获取多屏幕
//    QScreen * screen = QGuiApplication::primaryScreen();//获取主屏幕
    QScreen * screen = screens.at(0);//获取多屏幕第一块屏幕（暂未做多屏幕的兼容）
    QRect screenRect = screen->geometry();
    int screenWidth = screenRect.width();
    int screenHeight = screenRect.height();

    int initW = int(float(screenWidth) * 0.5);
    int initH = int(float(screenHeight) * 0.5);

    QLOG_INFO() << "MainWindow::MainWindow() screens="<<screens.size()<<",screenWidth="<<screenWidth<<",screenHeight="<<screenHeight<<",initW="<<initW<<",initH="<<initH;
    qDebug() << "MainWindow::MainWindow() screens="<<screens.size()<<",screenWidth="<<screenWidth<<",screenHeight="<<screenHeight<<",initW="<<initW<<",initH="<<initH;

    this->resize(initW,initH);

    IndexWidget *index = new IndexWidget(this);
    setCentralWidget(index);
}

