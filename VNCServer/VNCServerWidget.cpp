#include "VNCServerWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>


VNCServerWidget::VNCServerWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground,true);
    setStyleSheet(QString(".VNCServerWidget{background-color:%1;}").arg("rgb(25,27,38)"));
    initUi();
}

void VNCServerWidget::initUi(){

    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0,0,0,0);
    mainVLayout->setSpacing(0);

    //name start
    QWidget * nameWidget = new QWidget(this);
    nameWidget->setFixedHeight(80);
    QHBoxLayout *nameHLayout = new QHBoxLayout(nameWidget);
    nameHLayout->setContentsMargins(0,0,0,0);
    nameHLayout->setSpacing(0);
    QLabel *nameLabel = new QLabel(nameWidget);
    nameLabel->setFixedHeight(40);
    nameLabel->setCursor(Qt::PointingHandCursor);
    nameLabel->setStyleSheet(".QLabel{color:rgb(255,255,255);font-size:18px;border:1px solid rgb(76,76,76); border-radius: 2px;padding: 4px 8px;}");
    nameLabel->setText(tr("允许控制本设备"));
    nameHLayout->addSpacing(40);
    nameHLayout->addWidget(nameLabel);
    nameHLayout->addStretch(10);
    //name end

    //info start
    QWidget * infoWidget = new QWidget(this);
    infoWidget->setFixedHeight(100);
    QHBoxLayout *infoHLayout = new QHBoxLayout(infoWidget);
    infoHLayout->setContentsMargins(0,0,0,0);
    infoHLayout->setSpacing(0);

    //info-left
    QString qss_name = ".QLabel{color:rgb(200,200,200);font-size:18px;}";
    QString qss_value = ".QLabel{color:rgb(255,255,255);font-size:22px;}";
    QWidget * infoLeftWidget = new QWidget(infoWidget);
    infoLeftWidget->setFixedWidth(200);
    QVBoxLayout *infoLeftVLayout = new QVBoxLayout(infoLeftWidget);
    infoLeftVLayout->setContentsMargins(0,0,0,0);
    infoLeftVLayout->setSpacing(0);

    QLabel *infoLeft_name = new QLabel(infoLeftWidget);
    infoLeft_name->setStyleSheet(qss_name);
    infoLeft_name->setText("本设备IP地址");

    QLabel *infoLeft_ip = new QLabel(infoLeftWidget);
    infoLeft_ip->setStyleSheet(qss_value);
    infoLeft_ip->setText("127.0.0.1");


    infoLeftVLayout->addSpacing(20);
    infoLeftVLayout->addWidget(infoLeft_name);
    infoLeftVLayout->addSpacing(10);
    infoLeftVLayout->addWidget(infoLeft_ip);
    infoLeftVLayout->addStretch(10);

    //info-right
    QWidget * infoRightWidget = new QWidget(infoWidget);
    infoRightWidget->setFixedWidth(200);
    QVBoxLayout *infoRightVLayout = new QVBoxLayout(infoRightWidget);
    infoRightVLayout->setContentsMargins(0,0,0,0);
    infoRightVLayout->setSpacing(0);

    QLabel *infoRight_name = new QLabel(infoRightWidget);
    infoRight_name->setStyleSheet(qss_name);
    infoRight_name->setText("连接验证码");

    QLabel *infoRight_code = new QLabel(infoRightWidget);
    infoRight_code->setStyleSheet(qss_value);
    infoRight_code->setText("112400");


    infoRightVLayout->addSpacing(20);
    infoRightVLayout->addWidget(infoRight_name);
    infoRightVLayout->addSpacing(10);
    infoRightVLayout->addWidget(infoRight_code);
    infoRightVLayout->addStretch(10);

    infoHLayout->addStretch(10);
    infoHLayout->addWidget(infoLeftWidget);
    infoHLayout->addSpacing(60);
    infoHLayout->addWidget(infoRightWidget);
    infoHLayout->addStretch(10);
    //info end




//    mainVLayout->addWidget(label,0,Qt::AlignHCenter);
//    mainVLayout->addStretch(50);

    mainVLayout->addSpacing(80);
    mainVLayout->addWidget(nameWidget);
    mainVLayout->addSpacing(10);
    mainVLayout->addWidget(infoWidget);
    mainVLayout->addStretch(10);
}
