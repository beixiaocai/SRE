#include "DesktopControlWidget.h"
#include <QVBoxLayout>

DesktopControlWidget::DesktopControlWidget(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_StyledBackground,true);
    setStyleSheet(QString(".DesktopControlWidget{background-color:%1;}").arg("rgb(255,0,0)"));

    initUi();
}
void DesktopControlWidget::initUi(){
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0,0,0,0);
    mainVLayout->setSpacing(0);



}
