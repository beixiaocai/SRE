#include "Settings.h"
#include "Utils/ComLineWidget.h"
#include "Utils/ComMessageBox.h"
#include "style.h"
#include "constant.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#include <QtCore5Compat>
#endif


Settings::Settings(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::Dialog| Qt::WindowCloseButtonHint);
    setAttribute(Qt::WA_StyledBackground,true);
    setWindowTitle("设置");
    setFixedSize(660,340);

    mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0,0,0,0);
    mainVLayout->setSpacing(0);

    mainVLayout->addStretch(10);
    mainVLayout->addWidget(new ComLineWidget(this));
    initBottomUi();


}

void Settings::initBottomUi(){


    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setFixedHeight(50);
    QHBoxLayout *bottomHLayout = new QHBoxLayout(bottomWidget);
    bottomHLayout->setContentsMargins(0,0,0,0);
    bottomHLayout->setSpacing(0);

    QPushButton *okBtn = new QPushButton(bottomWidget);
    okBtn->setText("保存");
    okBtn->setStyleSheet(m_stylesheet_QPushButton_blue);
    okBtn->setCursor(Qt::PointingHandCursor);
    okBtn->setFixedSize(90,32);
    connect(okBtn,&QPushButton::clicked,this,[this](){

    });

    QPushButton *cancelBtn = new QPushButton(bottomWidget);
    cancelBtn->setText("取消");
    cancelBtn->setStyleSheet(m_stylesheet_QPushButton_hollow);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setFixedSize(90,32);
    connect(cancelBtn,&QPushButton::clicked,this,[this](){
        this->close();
    });



    bottomHLayout->addStretch(10);
    bottomHLayout->addWidget(okBtn);
    bottomHLayout->addSpacing(20);
    bottomHLayout->addWidget(cancelBtn);
    bottomHLayout->addSpacing(20);


    mainVLayout->addWidget(bottomWidget);

}
