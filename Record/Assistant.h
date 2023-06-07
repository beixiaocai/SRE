#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QObject>
#include <QImage>

class CaptureAudio;
class CaptureVideo;
class FFmpegEncoder;

class Assistant : public QObject
{
    Q_OBJECT
public:
    explicit Assistant(QObject *parent);
    ~Assistant();

public:
    int start();
    int pause();
    int stop();
private:
    bool mIsRecording = false;//是否在录制中

    FFmpegEncoder *mEncoder = nullptr;
    CaptureVideo  *mCaptureVideo = nullptr;//采集（屏幕或摄像头）
    CaptureAudio  *mCaptureAudio = nullptr;//采集（麦克风或声卡）

signals:
    void setImage(QImage image);
private slots:

};

#endif // ASSISTANT_H
