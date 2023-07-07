#include "IndexWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QStackedWidget>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include "ComLineWidget.h"
#include "ComMessageBox.h"
#include "constant.h"
#include "RecordWidget.h"
#include "About.h"
#include "Settings.h"
#include "VNCClientWidget.h"
#include "VNCServerWidget.h"
#include "Utils.h"
#include "SingletonUtils.h"

IndexWidget::IndexWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground,true);
    setStyleSheet(".IndexWidget{background-color:rgb(76,76,76);}");//31,33,42->76,76,76
    initUi();
    initSettings();

}
void IndexWidget::initUi(){
    QHBoxLayout *mainHLayout = new QHBoxLayout(this);
    mainHLayout->setContentsMargins(0,0,0,0);
    mainHLayout->setSpacing(0);

    QWidget *leftWidget = initLeftWidget();
    leftWidget->setFixedWidth(200);

    QWidget *rightWidget = initRightWidget();
    mainHLayout->addWidget(leftWidget);
    mainHLayout->addSpacing(1);
    mainHLayout->addWidget(rightWidget);
}
QWidget* IndexWidget::initLeftWidget(){
    QWidget * widget = new QWidget(this);
    widget->setStyleSheet(QString(".QWidget{background-color:%1;}").arg("rgb(43,46,56)"));
    QVBoxLayout *widgetVLayout = new QVBoxLayout(widget);
    widgetVLayout->setContentsMargins(0,0,0,0);
    widgetVLayout->setSpacing(0);

    QSize size(140,40);

    vncServerBtn = new QPushButton(widget);
    vncServerBtn->setCursor(Qt::PointingHandCursor);
    vncServerBtn->setStyleSheet(left_btn_no_background);
    vncServerBtn->setFixedSize(size);
    vncServerBtn->setText("远程协助");

    vncClientBtn = new QPushButton(widget);
    vncClientBtn->setCursor(Qt::PointingHandCursor);
    vncClientBtn->setStyleSheet(left_btn_no_background);
    vncClientBtn->setFixedSize(size);
    vncClientBtn->setText("远程控制");

    recordBtn = new QPushButton(widget);
    recordBtn->setCursor(Qt::PointingHandCursor);
    recordBtn->setStyleSheet(left_btn_no_background_selected);
    recordBtn->setFixedSize(size);
    recordBtn->setText("录制屏幕");

    connect(vncServerBtn,&QPushButton::clicked,this,[this](){
        vncServerBtn->setStyleSheet(left_btn_no_background_selected);
        vncClientBtn->setStyleSheet(left_btn_no_background);
        recordBtn->setStyleSheet(left_btn_no_background);

        this->rightStackedWidget->setCurrentWidget(vncServerWidget);

    });
    connect(vncClientBtn,&QPushButton::clicked,this,[this](){
        vncServerBtn->setStyleSheet(left_btn_no_background);
        vncClientBtn->setStyleSheet(left_btn_no_background_selected);
        recordBtn->setStyleSheet(left_btn_no_background);
        this->rightStackedWidget->setCurrentWidget(vncClientWidget);
    });
    connect(recordBtn,&QPushButton::clicked,this,[this](){
        vncServerBtn->setStyleSheet(left_btn_no_background);
        vncClientBtn->setStyleSheet(left_btn_no_background);
        recordBtn->setStyleSheet(left_btn_no_background_selected);
        this->rightStackedWidget->setCurrentWidget(recordWidget);
    });


    // 菜单start
    QMenu *settingsMenu = new QMenu(this);

    QAction *settingsAct = settingsMenu->addAction("设置");
    settingsAct->setShortcuts(QKeySequence::WhatsThis);
    connect(settingsAct, &QAction::triggered, this, [this]() {
        Settings dlg(this);
        dlg.exec();
    });

    QAction *aboutAct = settingsMenu->addAction("关于");
    aboutAct->setShortcuts(QKeySequence::Preferences);
    connect(aboutAct, &QAction::triggered, this, [this]() {
        About dlg(this);
        dlg.exec();
    });
    QAction *feedbackAct = settingsMenu->addAction("反馈");
    connect(feedbackAct, &QAction::triggered, this, []() {
        QDesktopServices::openUrl(QUrl(SRE_URL_FEEDBACK));
    });

    QAction *logoutAct = settingsMenu->addAction("退出");
    logoutAct->setShortcuts(QKeySequence::Quit);
    connect(logoutAct, &QAction::triggered, this, [this]() {
        switch (QMessageBox::information(this,"提示","确认要退出吗？","确定","取消",0)){
            case 0:{

                QApplication* app;
                app->quit();
                break;
            }
        }
    });



    settingsMenu->addAction(settingsAct);
    settingsMenu->addSeparator();
    settingsMenu->addAction(aboutAct);
    settingsMenu->addSeparator();
    settingsMenu->addAction(feedbackAct);
    settingsMenu->addSeparator();
    settingsMenu->addAction(logoutAct);
    // 菜单end

    QPushButton *settingsBtn = new QPushButton(widget);
    settingsBtn->setIcon(QIcon(":/res/images/icon/settings.png"));
    settingsBtn->setText("菜单");
    settingsBtn->setStyleSheet(".QPushButton {color:rgb(255,255,255);font-size:16px;border:1px solid rgb(76,76,76); border-radius: 3px;padding: 2px;}\
                               .QPushButton:hover {background-color: rgb(53,53,53);}\
                               .QPushButton:focus{outline: none;}");
    settingsBtn->setCursor(Qt::PointingHandCursor);
    settingsBtn->setFixedSize(100,40);
    connect(settingsBtn,&QPushButton::clicked,this,[this,settingsMenu](){
        settingsMenu->exec(QCursor::pos());
    });

    widgetVLayout->addSpacing(60);
    widgetVLayout->addWidget(vncServerBtn,0,Qt::AlignHCenter);
    widgetVLayout->addSpacing(20);
    widgetVLayout->addWidget(vncClientBtn,0,Qt::AlignHCenter);
    widgetVLayout->addSpacing(20);
    widgetVLayout->addWidget(recordBtn,0,Qt::AlignHCenter);
    widgetVLayout->addStretch(10);
    widgetVLayout->addWidget(settingsBtn,0,Qt::AlignHCenter);
    widgetVLayout->addSpacing(20);
    return widget;
}
QWidget* IndexWidget::initRightWidget(){
    QWidget * widget = new QWidget(this);
    QVBoxLayout *widgetVLayout = new QVBoxLayout(widget);
    widgetVLayout->setContentsMargins(0,0,0,0);
    widgetVLayout->setSpacing(0);

    // StackedWidget start
    rightStackedWidget = new QStackedWidget(widget);
    rightStackedWidget->setStyleSheet(QString(".QStackedWidget{background-color:%1;}").arg("rgb(43,46,56)"));

    recordWidget = new RecordWidget(this);
    vncServerWidget = new VNCServerWidget(this);
    vncClientWidget = new VNCClientWidget(this);

    rightStackedWidget->addWidget(recordWidget);
    rightStackedWidget->addWidget(vncServerWidget);
    rightStackedWidget->addWidget(vncClientWidget);

    rightStackedWidget->setCurrentWidget(recordWidget);
    // StackedWidget end

    widgetVLayout->addWidget(rightStackedWidget);

    return widget;
}
void IndexWidget::initSettings(){
    QSettings settings;

    QString dirVal = settings.value(SRE_SETTINGS_DIR).toString();
    if(dirVal.isEmpty()){
        dirVal = QApplication::applicationDirPath() + "/Record";
        Utils::mkDirs(dirVal);
        settings.setValue(SRE_SETTINGS_DIR,dirVal);
    }
    SingletonUtils::getInstance()->setRecordDir(dirVal);
}
