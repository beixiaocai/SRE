#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QImage>
#include "BXC_VideoRecorder.h"
#include "BXC_AudioRecorder.h"
#include "BXC_AvEncoder.h"
using namespace BXC_MediaLibrary;
class CaptureVideoThread;
class CaptureAudioThread;
struct CaptureVideoDevice{
    CaptureVideoDevice() = delete;
    CaptureVideoDevice(QString nickname):mNickname(nickname){

    }
    CaptureVideoDevice(bool use,bool isCamera,QString nickname,QString name,int fps):
        mUse(use),mIsCamera(isCamera),mNickname(nickname),mName(name),mFps(fps){

        }
public:
    bool isUse(){
        return mUse;
    }
    bool isCamera(){
        return mIsCamera;
    }
    QString getNickname(){
        return mNickname;
    }
    QString getName(){
        return mName;
    }

    int getFps(){
        return mFps;
    }

    int width;
    int height;
private:
    bool mUse = false;
    bool mIsCamera = false;
    QString mNickname;
    QString mName;
    int mFps;
};
struct CaptureAudioDevice{
    CaptureAudioDevice() = delete;
    CaptureAudioDevice(QString nickname):
        mUse(false),mNickname(nickname){

        }
    CaptureAudioDevice(bool use,QString nickname,QString name):
        mUse(use),mNickname(nickname),mName(name){

        }
public:
    bool isUse(){
        return mUse;
    }
    QString getNickname(){
        return mNickname;
    }
    QString getName(){
        return mName;
    }
private:
    bool mUse;
    QString mNickname;
    QString mName;
};
class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject *parent);
    ~Recorder();

public:
    bool start(CaptureVideoDevice* videoDevice,CaptureAudioDevice* audioDevice,const QString &url);
    bool pause();
    bool stop();

    BXC_VideoRecorder* getVideoRecorder();
    BXC_AudioRecorder* getAudioRecorder();
    BXC_AvEncoder* getAvEncoder();
    BXC_PixelFormat getPixelFormat();
private:
    CaptureVideoDevice *mVideoDevice = nullptr;
    CaptureAudioDevice *mAudioDevice = nullptr;
    CaptureVideoThread *mCaptureVideoThread = nullptr;
    CaptureAudioThread *mCaptureAudioThread = nullptr;
    BXC_VideoRecorder  *mVideoRecorder = nullptr;
    BXC_AudioRecorder  *mAudioRecorder = nullptr;
    BXC_AvEncoder      *mAvEncoder = nullptr;
    bool mIsStop = true;
    BXC_PixelFormat mPixelFormat = PIXEL_UNKNOWN;

signals:
    void setImage(QImage image);
private slots:

};

#endif // RECORDER_H
