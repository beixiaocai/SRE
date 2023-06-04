#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>

class AcqFFmpeg;
class AcqScreen;
class FFmpegEncoder;
class PreviewPlayer;

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject *parent,PreviewPlayer *previewPlayer);
    ~Recorder();

public:
    int start();
    int pause();
    int stop();
private:
    PreviewPlayer *mPreviewPlayer;
    bool mIsRecording = false;//是否在录制中

    FFmpegEncoder *mEncoder = nullptr;
    AcqScreen     *mAcqScreen = nullptr;//采集屏幕
    AcqFFmpeg     *mAcqMicrophone = nullptr;//采集麦克风

signals:

private slots:

};

#endif // RECORDER_H
