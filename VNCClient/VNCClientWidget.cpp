#include "VNCClientWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include "ComLineWidget.h"
#include "ClientDialog.h"

VNCClientWidget::VNCClientWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground,true);
    setStyleSheet(QString(".VNCClientWidget{background-color:%1;}").arg("rgb(25,27,38)"));

    clientDialog = new ClientDialog(this);

    initUi();

}
void VNCClientWidget::initUi(){
    QHBoxLayout *mainHLayout = new QHBoxLayout(this);
    mainHLayout->setContentsMargins(0,0,0,0);
    mainHLayout->setSpacing(0);

    QWidget *leftWidget = initLeftWidget();
    leftWidget->setFixedWidth(300);

    QWidget *rightWidget = initRightWidget();
    mainHLayout->addWidget(leftWidget);
//    ComLineWidget *line = new ComLineWidget(this,1);
//    line->setStyleSheet(".ComLineWidget{background-color:rgb(76,76,76);}");
//    mainHLayout->addWidget(line);
    mainHLayout->addWidget(rightWidget);
}
QWidget* VNCClientWidget::initLeftWidget(){
    QWidget * widget = new QWidget(this);
    QVBoxLayout *widgetVLayout = new QVBoxLayout(widget);
    widgetVLayout->setContentsMargins(0,0,0,0);
    widgetVLayout->setSpacing(0);

    //name strt
    QWidget * nameWidget = new QWidget(widget);
    nameWidget->setFixedHeight(80);
    QHBoxLayout *nameHLayout = new QHBoxLayout(nameWidget);
    nameHLayout->setContentsMargins(0,0,0,0);
    nameHLayout->setSpacing(0);
    QLabel *nameLabel = new QLabel(nameWidget);
    nameLabel->setFixedHeight(40);
    nameLabel->setCursor(Qt::PointingHandCursor);
    nameLabel->setStyleSheet(".QLabel{color:rgb(255,255,255);font-size:18px;border:1px solid rgb(76,76,76); border-radius: 2px;padding: 4px 8px;}");
    nameLabel->setText(tr("我的设备列表"));
    nameHLayout->addSpacing(40);
    nameHLayout->addWidget(nameLabel);
    nameHLayout->addStretch(10);
    //name end

    //list start
    QWidget * deviceWidget = new QWidget(widget);
    QHBoxLayout *deviceHLayout = new QHBoxLayout(deviceWidget);
    deviceHLayout->setContentsMargins(20,0,20,0);
    deviceHLayout->setSpacing(0);
    deviceListWidget = new QListWidget(deviceWidget);
    deviceListWidget->setStyleSheet("QListWidget{background-color:rgba(0,0,0,0);border:1px solid gray;padding:10px; font-size:18px;color:rgb(255,255,255); }"
                               "QListWidget::Item{padding:10px; }"
                               "QListWidget::Item:hover{background:rgb(54,98,180); }"
                               "QListWidget::item:selected{background:rgb(54,98,180); }"
                               "QListWidget::item:selected:!active{background:rgb(54,98,180); }"
                               );
    deviceListWidget->setFocusPolicy(Qt::NoFocus);//去除虚线框

//    deviceListWidget->setStyleSheet(".QListWidget{border:1px solid rgb(217,217,217); border-radius: 0px;padding: 2px;}");
//    deviceListWidget->setFixedHeight(200);

    deviceHLayout->addSpacing(10);
    deviceHLayout->addWidget(deviceListWidget);
    deviceHLayout->addSpacing(10);


    QListWidgetItem * item = new QListWidgetItem("test1",deviceListWidget);
    deviceListWidget->addItem(item);
    item = new QListWidgetItem("test2",deviceListWidget);
    deviceListWidget->addItem(item);
    //list end


    widgetVLayout->addSpacing(80);
    widgetVLayout->addWidget(nameWidget);
    widgetVLayout->addSpacing(10);
    widgetVLayout->addWidget(deviceWidget);
    widgetVLayout->addStretch(10);

    return widget;

}
QWidget* VNCClientWidget::initRightWidget(){
    QWidget * widget = new QWidget(this);
    QVBoxLayout *widgetVLayout = new QVBoxLayout(widget);
    widgetVLayout->setContentsMargins(0,0,0,0);
    widgetVLayout->setSpacing(0);


    QWidget * menuWidget = new QWidget(widget);
    menuWidget->setFixedHeight(80);
    QHBoxLayout *menuHLayout = new QHBoxLayout(menuWidget);
    menuHLayout->setContentsMargins(0,0,0,0);
    menuHLayout->setSpacing(0);

    QPushButton *deskControlBtn = new QPushButton(widget);
    deskControlBtn->setFixedSize(120,40);
    deskControlBtn->setCursor(Qt::PointingHandCursor);
    deskControlBtn->setText(tr("桌面控制"));

    QPushButton *deskViewBtn = new QPushButton(widget);
    deskViewBtn->setFixedSize(120,40);
    deskViewBtn->setCursor(Qt::PointingHandCursor);
    deskViewBtn->setText(tr("桌面观看"));

    connect(deskControlBtn,&QPushButton::clicked,this,[this](){
        clientDialog->show();
    });

    connect(deskViewBtn,&QPushButton::clicked,this,[this](){
        clientDialog->show();
    });

    menuHLayout->addStretch(10);
    menuHLayout->addWidget(deskControlBtn);
    menuHLayout->addSpacing(20);
    menuHLayout->addWidget(deskViewBtn);
    menuHLayout->addStretch(10);


    widgetVLayout->addSpacing(100);
    widgetVLayout->addWidget(menuWidget);
    widgetVLayout->addStretch(10);



    return widget;


}
