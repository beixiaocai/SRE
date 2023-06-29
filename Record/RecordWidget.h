#ifndef RECORDWIDGET_H
#define RECORDWIDGET_H

#include <QWidget>
#include <QImage>
#include <QDateTime>
#include <QVector>
#include "Recorder.h"
QT_BEGIN_NAMESPACE
class QTimer;
class QLabel;
class QPushButton;
class QMediaDevices;
QT_END_NAMESPACE

class RecordWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RecordWidget(QWidget *parent);
    ~RecordWidget();

private:
    void initUi();
    QLabel *imageLabel;//渲染录屏画面

    QPushButton *startBtn;
    QPushButton *stopBtn;
    QPushButton *pauseBtn;

    QLabel    *durationLabel;
    QTimer    *durationTimer;

    QWidget* initRecordWidget();
    QWidget* initRecordSourceWidget();

    QMediaDevices *mMediaDevices;
    QVector<CaptureVideoDevice*> mVideoDevices;
    int mSelectedVideoIndex = 0;
    QVector<CaptureAudioDevice*> mAudioDevices;
    int mSelectedAudioIndex = 0;

private:
    Recorder *mRecorder;
    bool      mIsRecord;//是否在录制中
    QDateTime mRecordStartDate;//开始录制的时间

signals:

public slots:
    void onSetImage(QImage image);
    void onStartBtn(bool checked);
    void onStopBtn(bool checked);
};

#endif // RECORDWIDGET_H
