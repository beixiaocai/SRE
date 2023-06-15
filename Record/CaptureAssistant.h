#ifndef CAPTUREASSISTANT_H
#define CAPTUREASSISTANT_H

#include <QObject>
#include <QImage>

class CaptureAudio;
class CaptureVideo;
class CaptureFFmpegEncoder;

enum CaptureDeviceType{
    CAPTURE_NONE=0, //未选择状态
    CAPTURE_VIDEO_SCREEN, //系统屏幕
    CAPTURE_VIDEO_CAMERA, //摄像头
    CAPTURE_AUDIO_SOUNDCARD,//系统声音
    CAPTURE_AUDIO_MICROPHONE,//麦克风
    CAPTURE_AUDIO_SOUNDCARD_MICROPHONE//系统声音+麦克风混音
};
struct CaptureDevice{
    CaptureDevice(CaptureDeviceType type,QString name,QString description);
public:
    CaptureDeviceType getType();
    QString getName();
    QString getDescription();
private:
    CaptureDeviceType mType;
    QString mName;
    QString mDescription;
};

class CaptureAssistant : public QObject
{
    Q_OBJECT
public:
    explicit CaptureAssistant(QObject *parent);
    ~CaptureAssistant();

public:
    int start(CaptureDevice* videoDevice,CaptureDevice* audioDevice);
    int pause();
    int stop();
    CaptureDevice* getVideoDevice();
    CaptureDevice* getAudioDevice();
private:
    bool mIsRecording = false;//是否在录制中

    CaptureFFmpegEncoder *mEncoder = nullptr;
    CaptureVideo  *mCaptureVideo = nullptr;//采集（屏幕或摄像头）
    CaptureAudio  *mCaptureAudio = nullptr;//采集（麦克风或声卡）

    CaptureDevice *mVideoDevice;
    CaptureDevice *mAudioDevice;

signals:
    void setImage(QImage image);
private slots:

};

#endif // CAPTUREASSISTANT_H
