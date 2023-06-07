#include "RecordWidget.h"
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QStringList>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include "ComLineWidget.h"
#include "Utils.h"
#include "Assistant.h"
#include "SingletonUtils.h"
#include <QDebug>
#include <QsLog.h>

RecordWidget::RecordWidget(QWidget *parent) : QWidget(parent)
{
    initUi();
    mAssist = new Assistant(this);
    connect(mAssist, &Assistant::setImage, this, &RecordWidget::onSetImage);

    isRecording = false;
    durationTimer = new QTimer(this);
    connect(durationTimer,&QTimer::timeout,this,[this](){

        int seconds = QDateTime::currentDateTime().toSecsSinceEpoch() - recordStartDate.toSecsSinceEpoch();
        durationLabel->setText(Utils::secondsToDurationStr(seconds));
//        durationLabel->setText(QTime::currentTime().toString());
    });
}
RecordWidget::~RecordWidget(){

}
void RecordWidget::initUi(){
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0,0,0,0);
    mainVLayout->setSpacing(0);

    //name strt
    QWidget * nameWidget = new QWidget(this);
    nameWidget->setFixedHeight(40);
    QHBoxLayout *nameHLayout = new QHBoxLayout(nameWidget);
    nameHLayout->setContentsMargins(0,0,0,0);
    nameHLayout->setSpacing(0);
    QLabel *nameLabel = new QLabel(nameWidget);
    nameLabel->setStyleSheet(".QLabel{color:rgb(255,255,255);font-size:20px;}");
    nameLabel->setText("录屏预览");
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
    QWidget * recordWidget = new QWidget(this);
    recordWidget->setFixedHeight(80);
    QHBoxLayout *recordHLayout = new QHBoxLayout(recordWidget);
    recordHLayout->setContentsMargins(0,0,0,0);
    recordHLayout->setSpacing(0);

    QWidget *recordSourceWidget = initRecordSourceWidget();

    startBtn = new QPushButton(recordWidget);
    startBtn->setFixedSize(120,40);
    startBtn->setCursor(Qt::PointingHandCursor);
    startBtn->setText("开始");
    pauseBtn = new QPushButton(recordWidget);
    pauseBtn->setFixedSize(120,40);
    pauseBtn->setCursor(Qt::PointingHandCursor);
    pauseBtn->setText("暂停");
    stopBtn = new QPushButton(recordWidget);
    stopBtn->setFixedSize(120,40);
    stopBtn->setCursor(Qt::PointingHandCursor);
    stopBtn->setText("停止");

    connect(startBtn,&QPushButton::clicked,this,[this](){
        if(isRecording){
            qDebug()<<"正在录制中，请先停止录制";
        }else{
            qDebug()<<"开始录制";
            startBtn->hide();
            pauseBtn->show();
            stopBtn->show();

            isRecording = true;
            recordStartDate = QDateTime::currentDateTime();
            durationTimer->start(1000);


            mAssist->start();

        }


    });

    connect(stopBtn,&QPushButton::clicked,this,[this](){
        if(isRecording){
            qDebug()<<"正在录制中，可以执行停止录制";
            startBtn->show();
            pauseBtn->hide();
            stopBtn->hide();

            isRecording = false;
            durationTimer->stop();

            mAssist->stop();

            durationLabel->setText(Utils::secondsToDurationStr(0));

        }else{
            qDebug()<<"暂未开始录制，无法执行停止";

        }


    });
    pauseBtn->hide();
    stopBtn->hide();

    recordHLayout->addWidget(recordSourceWidget);
    recordHLayout->addWidget(pauseBtn);
    recordHLayout->addSpacing(10);
    recordHLayout->addWidget(startBtn);
    recordHLayout->addSpacing(10);
    recordHLayout->addWidget(stopBtn);
    recordHLayout->addSpacing(40);
    // record end


    // bottom start
    QWidget * bottomWidget = new QWidget(this);
    bottomWidget->setFixedHeight(40);
    QHBoxLayout *bottomHLayout = new QHBoxLayout(bottomWidget);
    bottomHLayout->setContentsMargins(0,0,0,0);
    bottomHLayout->setSpacing(0);

    QLabel *durationNameLabel = new QLabel(bottomWidget);
    durationNameLabel->setStyleSheet(".QLabel{color:rgb(255,242,252);font-size:18px;}");
    durationNameLabel->setText("REC:");

    durationLabel = new QLabel(bottomWidget);
    durationLabel->setStyleSheet(".QLabel{color:rgb(255,242,252);font-size:18px;}");
    durationLabel->setText(Utils::secondsToDurationStr(0));

    QPushButton *videoBtn = new QPushButton(bottomWidget);
    videoBtn->setStyleSheet(".QPushButton {color:rgb(255,255,255);font-size:16px;border:1px solid rgb(76,76,76); border-radius: 3px;padding: 2px;}\
                            .QPushButton:pressed {background-color: rgba(64,65,66,0.5);}\
                            .QPushButton:hover {background-color: rgba(64,65,66,0.5);}\
                            .QPushButton:focus{outline: none;}");
    videoBtn->setFixedSize(100,30);
    videoBtn->setCursor(Qt::PointingHandCursor);
    videoBtn->setText("视频列表");
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

QWidget* RecordWidget::initRecordSourceWidget(){

    QWidget * widget = new QWidget(this);
    QHBoxLayout *widgetHLayout = new QHBoxLayout(widget);
    widgetHLayout->setContentsMargins(0,0,0,0);
    widgetHLayout->setSpacing(0);

    QString record_source_QComboBox = ".QComboBox {color:rgb(64,65,66);font-size:16px; border:1px solid rgb(217,217,217);padding-left: 5px;}\
             .QComboBox:hover {border-color:rgb(64,65,66);}\
             .QComboBox::drop-down{width:2px;} \
             .QComboBox QAbstractItemView{border: 1px solid darkgray;border-radius:2px;outline:0px;} \
             .QComboBox QAbstractItemView::item { height: 26px;}\
             .QComboBox QAbstractItemView::item:selected {background-color:rgb(54,98,180);}";



    //视频源
    QComboBox * videoSourceBox = new QComboBox(widget);
    videoSourceBox->setStyleSheet(record_source_QComboBox);
    videoSourceBox->setFixedSize(120,36);
    QStyledItemDelegate *videoSourceDelegate = new QStyledItemDelegate(videoSourceBox);
    videoSourceBox->setItemDelegate(videoSourceDelegate);
    videoSourceBox->clear();

    QStringList videoSources = {"屏幕","摄像头"};
    for (int i = 0; i < videoSources.size(); ++i) {

        videoSourceBox->addItem(videoSources[i]);
    }

    connect(videoSourceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index){
        qDebug()<<"sourceBox index="<<index;

    });
    //音频源
    QComboBox * audioSourceBox = new QComboBox(widget);
    audioSourceBox->setStyleSheet(record_source_QComboBox);
    audioSourceBox->setFixedSize(120,36);
    QStyledItemDelegate *audioSourceDelegate = new QStyledItemDelegate(audioSourceBox);
    audioSourceBox->setItemDelegate(audioSourceDelegate);
    audioSourceBox->clear();

    QStringList audioSources = {"系统声音","麦克风"};
    for (int i = 0; i < audioSources.size(); ++i) {

        audioSourceBox->addItem(audioSources[i]);
    }

    connect(audioSourceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index){
        qDebug()<<"sourceBox index="<<index;

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
