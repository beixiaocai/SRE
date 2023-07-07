#include "ClientDialog.h"
#include "ClientTabWidget.h"
#include <QVBoxLayout>
#include <QsLog.h>
#include <QDebug>
ClientDialog::ClientDialog(QWidget *parent) :QDialog(parent)
{
    qDebug()<<"ClientDialog::ClientDialog()";
    Qt::WindowFlags flags=Qt::Dialog;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    resize(1000,600);
    setAttribute(Qt::WA_StyledBackground,true);
    setStyleSheet(QString(".ClientDialog{background-color:%1;}").arg("rgb(43,46,56)"));
    setWindowTitle(tr("远程控制"));

    initUi();

}

ClientDialog::~ClientDialog(){
    qDebug()<<"ClientDialog::~ClientDialog()";

}
void ClientDialog::initUi(){
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0,0,0,0);
    mainVLayout->setSpacing(0);

    CilentTabWidget *tabWidget = new CilentTabWidget(this);
    mainVLayout->addWidget(tabWidget);

    tabWidget->addDesktopTab();

}

void ClientDialog::closeEvent(QCloseEvent *event){
    Q_UNUSED(event);

}
