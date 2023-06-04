#include "PreviewPlayer.h"
#include <QVBoxLayout>
#include <QImage>
#include <QLabel>

PreviewPlayer::PreviewPlayer(QWidget *parent) : QWidget(parent)
{
    initUi();
    connect(this, &PreviewPlayer::setImage, this, &PreviewPlayer::onSetImage);
}
void PreviewPlayer::initUi(){

    mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(10,0,10,10);
    mainVLayout->setSpacing(0);


    QWidget *nameWidget = initNameWidget();
    QWidget *imageWidget = initImageWidget();

    mainVLayout->addWidget(nameWidget);
    mainVLayout->addWidget(imageWidget);

}
QWidget* PreviewPlayer::initNameWidget(){
    QWidget * widget = new QWidget(this);
    widget->setFixedHeight(40);
    QHBoxLayout *widgetHLayout = new QHBoxLayout(widget);
    widgetHLayout->setContentsMargins(0,0,0,0);
    widgetHLayout->setSpacing(0);

    QLabel *nameLabel = new QLabel(widget);
    nameLabel->setStyleSheet(".QLabel{color:rgb(255,255,255);font-size:20px;}");
    nameLabel->setText("预览");

    widgetHLayout->addStretch(10);
    widgetHLayout->addWidget(nameLabel);
    widgetHLayout->addStretch(10);

    return widget;

}
QWidget* PreviewPlayer::initImageWidget(){

    QWidget * widget = new QWidget(this);
    QHBoxLayout *widgetHLayout = new QHBoxLayout(widget);
    widgetHLayout->setContentsMargins(0,0,0,0);
    widgetHLayout->setSpacing(0);

    imageLabel = new QLabel(widget);
    imageLabel->setFixedSize(960,540);
    imageLabel->setStyleSheet(".QLabel {border:2px solid rgb(76,76,76); border-radius: 3px;}");


    widgetHLayout->addStretch(10);
    widgetHLayout->addWidget(imageLabel);
    widgetHLayout->addStretch(10);

    return widget;
}

void PreviewPlayer::onSetImage(QImage image)
{

//    update(); //调用update将执行 paintEvent函数

    QImage scaled_image = image.scaled(imageLabel->width(), imageLabel->height(), Qt::IgnoreAspectRatio);

//    qDebug()<<"PreviewPlayer::onSetImage width="<<this->width()<<",height="<<this->height()<<
//              ",image.width="<<image.width()<<",image.height="<<image.height()<<
//              ",scaled_image.width="<<scaled_image.width()<<",scaled_image.height="<<scaled_image.height();

    QPixmap pixmap = QPixmap::fromImage(scaled_image);
    imageLabel->setPixmap(pixmap);

}
