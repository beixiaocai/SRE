#include "RecordWidget.h"
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QStringList>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QDesktopServices>
#include <QVariant>
#include <QUrl>
#include "ComMessageBox.h"
#include "ComLineWidget.h"
#include "Utils.h"
#include "SingletonUtils.h"
#include <QDebug>
#include <QsLog.h>
#include <QAudioDevice>
#include <QAudioInput>
#include <QCameraDevice>
#include <QMediaDevices>

RecordWidget::RecordWidget(QWidget *parent) : QWidget(parent)
{

    initUi();

    mAssistant = new CaptureAssistant(this);

    connect(mAssistant, &CaptureAssistant::setImage, this, &RecordWidget::onSetImage);

    mIsRecord = false;
    durationTimer = new QTimer(this);
    connect(durationTimer,&QTimer::timeout,this,[this](){

        int seconds = QDateTime::currentDateTime().toSecsSinceEpoch() - mRecordStartDate.toSecsSinceEpoch();
        durationLabel->setText(Utils::secondsToDurationStr(seconds));
//        durationLabel->setText(QTime::currentTime().toString());
    });


}
RecordWidget::~RecordWidget(){
    disconnect(mAssistant, &CaptureAssistant::setImage, this, &RecordWidget::onSetImage);
    if(mAssistant){
        delete mAssistant;
        mAssistant = nullptr;
    }
    if(mMediaDevices){
        delete mMediaDevices;
        mMediaDevices = nullptr;
    }
    for (int i = 0; i < mVideoDevices.size(); ++i) {
        CaptureDevice *device = mVideoDevices[i];
        delete device;
        device = nullptr;
    }
    for (int i = 0; i < mAudioDevices.size(); ++i) {
        CaptureDevice *device = mAudioDevices[i];
        delete device;
        device = nullptr;
    }
    mVideoDevices.clear();
    mAudioDevices.clear();

}
void RecordWidget::initUi(){
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0,0,0,0);
    mainVLayout->setSpacing(0);

    //name strt
    QWidget * nameWidget = new QWidget(this);
    nameWidget->setFixedHeight(50);
    QHBoxLayout *nameHLayout = new QHBoxLayout(nameWidget);
    nameHLayout->setContentsMargins(0,0,0,0);
    nameHLayout->setSpacing(0);
    QLabel *nameLabel = new QLabel(nameWidget);
    nameLabel->setFixedHeight(30);
    nameLabel->setStyleSheet(".QLabel{color:rgb(255,255,255);font-size:15px;border:1px solid rgb(76,76,76); border-radius: 2px;padding: 4px 8px;}");
    nameLabel->setText(tr("录屏预览"));
    nameHLayout->addStretch(10);
    nameHLayout->addWidget(nameLabel);
    nameHLayout->addStretch(10);
    //name end

    //image start
    QWidget *imageWidget = new QWidget(this);
    imageWidget->setStyleSheet(QString(".QWidget{background-color:%1;}").arg("rgb(25,27,38)"));
    QHBoxLayout *imageHLayout = new QHBoxLayout(imageWidget);
    imageHLayout->setContentsMargins(0,0,0,0);
    imageHLayout->setSpacing(0);

    imageLabel = new QLabel(imageWidget);
    imageLabel->setFixedSize(880,500);
    imageHLayout->addStretch(10);
    imageHLayout->addWidget(imageLabel);
    imageHLayout->addStretch(10);
    //image end

    // record start
    QWidget * recordWidget = initRecordWidget();
    // record end


    // bottom start
    QWidget * bottomWidget = new QWidget(this);
    bottomWidget->setFixedHeight(40);
    QHBoxLayout *bottomHLayout = new QHBoxLayout(bottomWidget);
    bottomHLayout->setContentsMargins(0,0,0,0);
    bottomHLayout->setSpacing(0);

    QLabel *durationNameLabel = new QLabel(bottomWidget);
    durationNameLabel->setStyleSheet(".QLabel{color:rgb(255,242,252);font-size:16px;}");
    durationNameLabel->setText("REC:");

    durationLabel = new QLabel(bottomWidget);
    durationLabel->setStyleSheet(".QLabel{color:rgb(255,242,252);font-size:18px;}");
    durationLabel->setText(Utils::secondsToDurationStr(0));

    QPushButton *videoBtn = new QPushButton(bottomWidget);
    videoBtn->setStyleSheet(".QPushButton {color:rgb(255,255,255);font-size:15px;border:1px solid rgb(76,76,76); border-radius: 3px;padding: 2px;}\
                            .QPushButton:pressed {background-color: rgba(64,65,66,0.5);}\
                            .QPushButton:hover {background-color: rgba(64,65,66,0.5);}\
                            .QPushButton:focus{outline: none;}");
    videoBtn->setFixedSize(100,30);
    videoBtn->setCursor(Qt::PointingHandCursor);
    videoBtn->setText(tr("录制列表"));
    connect(videoBtn,&QPushButton::clicked,this,[this](){
        QDesktopServices::openUrl(QUrl::fromLocalFile(SingletonUtils::getInstance()->getRecordDir()));

    });

    bottomHLayout->addSpacing(20);
    bottomHLayout->addWidget(durationNameLabel);
    bottomHLayout->addSpacing(10);
    bottomHLayout->addWidget(durationLabel);
    bottomHLayout->addStretch(10);
    bottomHLayout->addWidget(videoBtn);
    bottomHLayout->addSpacing(60);
    // bottom end


    mainVLayout->addWidget(nameWidget);
    mainVLayout->addWidget(imageWidget);
    mainVLayout->addWidget(recordWidget);
    ComLineWidget*line = new ComLineWidget(this);
    line->setStyleSheet(QString(".ComLineWidget{background-color:%1;}").arg("rgb(25,27,38)"));
    mainVLayout->addWidget(line);

    mainVLayout->addWidget(bottomWidget);
    ComLineWidget*line2 = new ComLineWidget(this);
    line2->setStyleSheet(QString(".ComLineWidget{background-color:%1;}").arg("rgb(25,27,38)"));
    mainVLayout->addWidget(line2);

    mainVLayout->addStretch(10);
}
QWidget* RecordWidget::initRecordWidget(){
    QWidget * widget = new QWidget(this);
    widget->setFixedHeight(80);
    QHBoxLayout *widgetHLayout = new QHBoxLayout(widget);
    widgetHLayout->setContentsMargins(0,0,0,0);
    widgetHLayout->setSpacing(0);

    QWidget *recordSourceWidget = initRecordSourceWidget();

    startBtn = new QPushButton(widget);
    startBtn->setFixedSize(120,40);
    startBtn->setCursor(Qt::PointingHandCursor);
    startBtn->setText(tr("开始"));
    pauseBtn = new QPushButton(widget);
    pauseBtn->setFixedSize(120,40);
    pauseBtn->setCursor(Qt::PointingHandCursor);
    pauseBtn->setText(tr("暂停"));
    stopBtn = new QPushButton(widget);
    stopBtn->setFixedSize(120,40);
    stopBtn->setCursor(Qt::PointingHandCursor);
    stopBtn->setText(tr("停止"));

    connect(startBtn,&QPushButton::clicked,this,[this](){
        if(mIsRecord){
            qDebug()<<"startBtn click, current mIsRecord=true";
        }else{
            qDebug()<<"startBtn click, current mIsRecord=false";
            CaptureDevice *videoDevice = mVideoDevices[mSelectedVideoIndex];
            CaptureDevice *audioDevice = mAudioDevices[mSelectedAudioIndex];
            if(CAPTURE_NONE==videoDevice->getType() && CAPTURE_NONE==audioDevice->getType()){
                ComMessageBox::error(this,"请至少选择一种输入源!");
                return;
            }

            startBtn->hide();
            pauseBtn->show();
            stopBtn->show();

            mIsRecord = true;
            mRecordStartDate = QDateTime::currentDateTime();
            durationTimer->start(1000);



            if(0!=mAssistant->start(videoDevice,audioDevice)){
                qDebug()<<"start record error";
            }
        }


    });

    connect(stopBtn,&QPushButton::clicked,this,[this](){
        if(mIsRecord){
            qDebug()<<"stopBtn click, current mIsRecord=true";
            startBtn->show();
            pauseBtn->hide();
            stopBtn->hide();

            mIsRecord = false;
            durationTimer->stop();

            mAssistant->stop();

            durationLabel->setText(Utils::secondsToDurationStr(0));


        }else{
            qDebug()<<"stopBtn click, current mIsRecord=false";

        }


    });
    pauseBtn->hide();
    stopBtn->hide();

    widgetHLayout->addWidget(recordSourceWidget);
    widgetHLayout->addWidget(pauseBtn);
    widgetHLayout->addSpacing(10);
    widgetHLayout->addWidget(startBtn);
    widgetHLayout->addSpacing(10);
    widgetHLayout->addWidget(stopBtn);
    widgetHLayout->addSpacing(40);

    return widget;
}
QWidget* RecordWidget::initRecordSourceWidget(){
    mMediaDevices = new QMediaDevices(this);

    mVideoDevices.append(new CaptureDevice(CAPTURE_NONE,"请选择视频源","请选择视频源"));
    mVideoDevices.append(new CaptureDevice(CAPTURE_VIDEO_SCREEN,"系统屏幕","系统屏幕"));
    for (auto &device : mMediaDevices->videoInputs()) {
        QString description = device.description();
        QString name = device.description();
        if(name.length()> 10){
            name = name.mid(0,10);
        }
        mVideoDevices.append(new CaptureDevice(CAPTURE_VIDEO_CAMERA,name,description));
    }
    mAudioDevices.append(new CaptureDevice(CAPTURE_NONE,"请选择音频源","请选择音频源"));
    mAudioDevices.append(new CaptureDevice(CAPTURE_AUDIO_SOUNDCARD,"系统声音","系统声音"));
    for (auto &device : mMediaDevices->audioInputs()) {
        QString description = device.description();
        QString name = device.description();
        if(name.length()> 10){
            name = name.mid(0,10);
        }
        mAudioDevices.append(new CaptureDevice(CAPTURE_AUDIO_MICROPHONE,name,description));
    }

    QWidget * widget = new QWidget(this);
    QHBoxLayout *widgetHLayout = new QHBoxLayout(widget);
    widgetHLayout->setContentsMargins(0,0,0,0);
    widgetHLayout->setSpacing(0);

    QString record_source_QComboBox = ".QComboBox {color:rgb(0,0,0);background-color:rgb(255,255,255);font-size:16px;border:1px solid rgb(217,217,217);}\
             .QComboBox:hover {border:1px solid rgb(54,98,180);}\
             .QComboBox::drop-down{width:0px;} \
             .QComboBox QAbstractItemView{outline:0px;} \
             .QComboBox QAbstractItemView::item {font-size:14px;background-color:rgb(255,255,255);height: 28px;border-bottom:1px solid rgb(217,217,217);}\
             .QComboBox QAbstractItemView::item:selected {color:rgb(0,0,0);}";


    //视频源
    QComboBox * videoSourceBox = new QComboBox(widget);
    videoSourceBox->setStyleSheet(record_source_QComboBox);
    videoSourceBox->setFixedSize(180,36);
    QStyledItemDelegate *videoSourceDelegate = new QStyledItemDelegate(videoSourceBox);
    videoSourceBox->setItemDelegate(videoSourceDelegate);
    videoSourceBox->clear();

    //音频源
    QComboBox * audioSourceBox = new QComboBox(widget);
    audioSourceBox->setStyleSheet(record_source_QComboBox);
    audioSourceBox->setFixedSize(180,36);
    QStyledItemDelegate *audioSourceDelegate = new QStyledItemDelegate(audioSourceBox);
    audioSourceBox->setItemDelegate(audioSourceDelegate);
    audioSourceBox->clear();

    for (int i = 0; i < mVideoDevices.size(); ++i) {
        CaptureDevice* device = mVideoDevices[i];
        videoSourceBox->addItem(device->getName());
    }
    connect(videoSourceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index){
        mSelectedVideoIndex = index;

    });
    for (int i = 0; i < mAudioDevices.size(); ++i) {
        CaptureDevice *device = mAudioDevices[i];
        audioSourceBox->addItem(device->getName());
    }
    connect(audioSourceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index){
        mSelectedAudioIndex = index;
    });
    //添加控件
    widgetHLayout->addSpacing(20);
    widgetHLayout->addWidget(videoSourceBox);
    widgetHLayout->addSpacing(20);
    widgetHLayout->addWidget(audioSourceBox);

    widgetHLayout->addStretch(10);


    return widget;
}
void RecordWidget::onSetImage(QImage image)
{

//    update(); //调用update将执行 paintEvent函数
    int scaled_h = imageLabel->height();
    int scaled_w = int(float(scaled_h) * float(image.width())/float(image.height()));

    QImage scaled_image = image.scaled(scaled_w, scaled_h, Qt::IgnoreAspectRatio);

//    qDebug()<<"RecordWidget::onSetImage width="<<this->width()<<",height="<<this->height()<<
//              ",image.width="<<image.width()<<",image.height="<<image.height()<<
//              ",scaled_image.width="<<scaled_image.width()<<",scaled_image.height="<<scaled_image.height();

    QPixmap pixmap = QPixmap::fromImage(scaled_image);
    imageLabel->setPixmap(pixmap);

}
