#include "Settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QFileDialog>
#include <QStackedWidget>
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#include <QtCore5Compat>
#endif
#include "ComLineWidget.h"
#include "ComMessageBox.h"
#include "constant.h"
#include "SingletonUtils.h"

Settings::Settings(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::Dialog| Qt::WindowCloseButtonHint);
    setAttribute(Qt::WA_StyledBackground,true);
    setWindowTitle("设置");
    setFixedSize(900,600);

    this->setStyleSheet(QString(".Settings{background-color:%1;}").arg("rgb(31,33,42)"));

    initUi();

}
void Settings::initUi(){
    QHBoxLayout *mainHLayout = new QHBoxLayout(this);
    mainHLayout->setContentsMargins(0,0,0,0);
    mainHLayout->setSpacing(0);


    QWidget *leftWidget = initLeftWidget();
    leftWidget->setFixedWidth(200);
    QWidget *rightWidget = initRightWidget();

    mainHLayout->addWidget(leftWidget);
    mainHLayout->addSpacing(2);
    mainHLayout->addWidget(rightWidget);
}
QWidget* Settings::initLeftWidget(){
    QWidget * widget = new QWidget(this);
    widget->setStyleSheet(QString(".QWidget{background-color:%1;}").arg("rgb(43,46,56)"));
    QVBoxLayout *widgetVLayout = new QVBoxLayout(widget);
    widgetVLayout->setContentsMargins(0,0,0,0);
    widgetVLayout->setSpacing(0);


    QPushButton *commonBtn = new QPushButton(widget);//通用
    commonBtn->setStyleSheet(".QPushButton {color:white;font-size:16px;background-color:rgb(43,113,237);border:none; border-radius: 2px;padding: 2px;}\
                             .QPushButton:pressed {background-color: rgba(43,113,237,0.5);}\
                             .QPushButton:hover {background-color: rgba(43,113,237,0.5);}\
                             .QPushButton:focus{outline: none;}");
    commonBtn->setFixedSize(120,40);
    commonBtn->setText("通用");

    widgetVLayout->addSpacing(10);
    widgetVLayout->addWidget(commonBtn,0,Qt::AlignHCenter);
    widgetVLayout->addStretch(10);


    return widget;
}
QWidget* Settings::initRightWidget(){
    QWidget * widget = new QWidget(this);
    QVBoxLayout *widgetVLayout = new QVBoxLayout(widget);
    widgetVLayout->setContentsMargins(0,0,0,0);
    widgetVLayout->setSpacing(0);

    rightStackedWidget = new QStackedWidget(widget);
    rightStackedWidget->setContentsMargins(10,0,10,0);
    initCommonWidget();

    //bottom start
    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setFixedHeight(50);
    QHBoxLayout *bottomHLayout = new QHBoxLayout(bottomWidget);
    bottomHLayout->setContentsMargins(0,0,0,0);
    bottomHLayout->setSpacing(0);

    // 背景为浅灰色，无边框，白色字
    const QString right_bottom_btn = ".QPushButton {color:white;font-size:16px;background-color:rgb(60,64,75);border:none; border-radius: 3px;padding: 2px;}\
             .QPushButton:hover {background-color: rgb(70,74,85);}\
             .QPushButton:focus{outline: none;}";

    QPushButton *okBtn = new QPushButton(bottomWidget);
    okBtn->setText("保存");
    okBtn->setStyleSheet(right_bottom_btn);
    okBtn->setCursor(Qt::PointingHandCursor);
    okBtn->setFixedSize(100,40);
    connect(okBtn,&QPushButton::clicked,this,[this](){

    });

    QPushButton *cancelBtn = new QPushButton(bottomWidget);
    cancelBtn->setText("取消");
    cancelBtn->setStyleSheet(right_bottom_btn);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setFixedSize(100,40);
    connect(cancelBtn,&QPushButton::clicked,this,[this](){
        this->close();
    });



    bottomHLayout->addStretch(10);
    bottomHLayout->addWidget(okBtn);
    bottomHLayout->addSpacing(20);
    bottomHLayout->addWidget(cancelBtn);
    bottomHLayout->addSpacing(20);
    //bottom end


    widgetVLayout->addWidget(rightStackedWidget);
    widgetVLayout->addSpacing(2);
    widgetVLayout->addWidget(bottomWidget);
    return widget;
}

void Settings::initCommonWidget(){

    QSettings settings;
    QWidget * widget = new QWidget(rightStackedWidget);
    QVBoxLayout *widgetVLayout = new QVBoxLayout(widget);
    widgetVLayout->setContentsMargins(0,0,0,0);
    widgetVLayout->setSpacing(0);

    //dir start
    QWidget *dirWidget = new QWidget(widget);
    dirWidget->setStyleSheet(QString(".QWidget{background-color:%1;}").arg("rgb(43,46,56)"));
    dirWidget->setFixedHeight(50);
    QHBoxLayout *dirHLayout = new QHBoxLayout(dirWidget);
    dirHLayout->setContentsMargins(0,0,0,0);
    dirHLayout->setSpacing(0);

    QLabel *dirNameLabel = new QLabel(dirWidget);
    dirNameLabel->setFixedWidth(60);
    dirNameLabel->setStyleSheet(".QLabel{color:rgb(255,255,255);font-size:15px;}");
    dirNameLabel->setText("录像路径");

    QLineEdit *dirLineEdit = new QLineEdit(dirWidget);
    dirLineEdit->setFixedHeight(32);
    dirLineEdit->setPlaceholderText("请输入录像路径");
    dirLineEdit->setStyleSheet(".QLineEdit{color:rgb(0,0,0);font-size:15px;border:1px solid rgb(217,217,217);border-radius: 3px; padding: 1px;}\
                               .QLineEdit:hover {border:1px solid rgb(64,65,66);}");
    dirLineEdit->setClearButtonEnabled(true);
    dirLineEdit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    dirLineEdit->setText(SingletonUtils::getInstance()->getRecordDir());
    dirLineEdit->setCursorPosition(0);

    QPushButton *dirBtn= new QPushButton(dirWidget);
    dirBtn->setStyleSheet( ".QPushButton {color:rgb(64,65,66);font-family:Microsoft YaHei;font-size:14px;background-color:white; border:1px solid rgb(64,65,66); padding: 0px;}\
                           .QPushButton:pressed {background-color: rgba(240,240,240,0.8);}\
                           .QPushButton:hover {background-color: rgba(240,240,240,0.4); border-color:rgba(64,65,66,0.5);}\
                           .QPushButton:focus{outline: none;}");
    dirBtn->setFixedSize(100,40);
    dirBtn->setText("选择");
    connect(dirBtn,&QPushButton::clicked,this,[this,dirLineEdit](){
        QString dirVal = QFileDialog::getExistingDirectory(this,"浏览","").trimmed();
        if(dirVal.length()>0){
            dirLineEdit->setText(dirVal);
            dirLineEdit->setCursorPosition(0);
            QSettings settings;
            settings.setValue(SRE_SETTINGS_DIR,dirVal);
        }

    });
    dirHLayout->addSpacing(10);
    dirHLayout->addWidget(dirNameLabel);
    dirHLayout->addSpacing(10);
    dirHLayout->addWidget(dirLineEdit);
    dirHLayout->addSpacing(10);
    dirHLayout->addWidget(dirBtn);
    dirHLayout->addSpacing(10);
    //dir end

    widgetVLayout->addSpacing(10);
    widgetVLayout->addWidget(dirWidget);
    widgetVLayout->addStretch(10);


    rightStackedWidget->addWidget(widget);
    rightStackedWidget->setCurrentWidget(widget);
}
