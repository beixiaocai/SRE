#include "Recorder.h"
#include "CaptureVideoThread.h"
#include "CaptureAudioThread.h"
#include "SingletonUtils.h"
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QsLog.h>
/*
* Qt跨平台宏 https://zhuanlan.zhihu.com/p/619580373
*/

Recorder::Recorder(QObject *parent): QObject(parent)
{
}
Recorder::~Recorder(){
    this->stop();
}

bool Recorder::start(CaptureVideoDevice* videoDevice,CaptureAudioDevice* audioDevice,const QString &url){
    mVideoDevice = videoDevice;
    mAudioDevice = audioDevice;

    //采集视频相关参数
    int videoBitrate = 2000000;
    mVideoDevice->width = SingletonUtils::getInstance()->getScreenWidth();
    mVideoDevice->height = SingletonUtils::getInstance()->getScreenHeight();

    int fps = mVideoDevice->getFps();
    bool hasVideo = mVideoDevice->isUse();
    const char* videoCodecName = "libx264";//h264_qsv,libx264,h264_nvenc,h264_amf

    //采集音频相关参数
    bool hasAudio = mAudioDevice->isUse();
    const char* audioCodecName = "aac";//aac,libmp3lame
    int audioBitrate = 128000;

    int ret;

    if(hasVideo){
        const char * videoCapture = mVideoDevice->getName().toStdString().data();
        mVideoRecorder = new BXC_VideoRecorder(videoCapture,mVideoDevice->width,mVideoDevice->height,0);

        ret = BXC_VideoRecorder_Open(mVideoRecorder);
        if (ret < 0) {
            return -1;
        }

        mVideoDevice->width = mVideoRecorder->factWidth;
        mVideoDevice->height = mVideoRecorder->factHeight;
        mPixelFormat = mVideoRecorder->pixelFormat;

    }
    if(hasAudio){
        const char * audioCapture = mAudioDevice->getName().toStdString().data();
        mAudioRecorder = new BXC_AudioRecorder(audioCapture);

        ret = BXC_AudioRecorder_Open(mAudioRecorder);
        if (ret < 0) {
            return -1;
        }
    }

    mAvEncoder = new BXC_AvEncoder(hasVideo, videoCodecName, videoBitrate, mPixelFormat,
                                   mVideoDevice->width, mVideoDevice->height, fps,
                                   hasAudio, audioCodecName, audioBitrate);

    ret = BXC_AvEncoder_Open(mAvEncoder,url.toLatin1().constData());
    if (ret < 0) {
        return -1;
    }

    mIsStop = false;
    //开启采集视频帧线程
    if(hasVideo){
        mCaptureVideoThread = new CaptureVideoThread(this,mVideoDevice);
        mCaptureVideoThread->start(QThread::NormalPriority);
    }

    //开启采集音频帧线程
    if(hasAudio){
        mCaptureAudioThread = new CaptureAudioThread(this,mAudioDevice);
        mCaptureAudioThread->start(QThread::NormalPriority);
    }

    return true;

}
bool Recorder::pause(){
    return false;
}
bool Recorder::stop(){
    mIsStop = true;

    if(mCaptureVideoThread){
        delete mCaptureVideoThread;
        mCaptureVideoThread = nullptr;
    }
    if(mCaptureAudioThread){
        delete mCaptureAudioThread;
        mCaptureAudioThread = nullptr;
    }

    if(mVideoRecorder){
        BXC_VideoRecorder_Close(mVideoRecorder);
        delete mVideoRecorder;
        mVideoRecorder = nullptr;
    }

    if(mAudioRecorder){
        BXC_AudioRecorder_Close(mAudioRecorder);
        delete mAudioRecorder;
        mAudioRecorder = nullptr;
    }

    if(mAvEncoder){
        BXC_AvEncoder_Close(mAvEncoder);
        delete mAvEncoder;
        mAvEncoder = nullptr;
    }

    if(mVideoDevice && mVideoDevice->isUse()){

        QTimer::singleShot(100,this,[this](){
            QImage image(mVideoDevice->width,mVideoDevice->height,QImage::Format_RGB32);
            image.fill(QColor(25,27,38));
            emit this->setImage(image.copy());
        });
    }
//    std::thread([](Recorder *recorder){}, this).detach();
    return true;
}
BXC_VideoRecorder* Recorder::getVideoRecorder(){
    return mVideoRecorder;
}
BXC_PixelFormat Recorder::getPixelFormat(){
    return mPixelFormat;
}
BXC_AudioRecorder* Recorder::getAudioRecorder(){
    return mAudioRecorder;
}
 BXC_AvEncoder* Recorder::getAvEncoder(){
    return mAvEncoder;
}
