#include "CaptureAssistant.h"
#include "CaptureFFmpegEncoder.h"
#include "CaptureVideo.h"
#include "CaptureAudio.h"
#include <thread>
#include "SingletonUtils.h"
#include <QDateTime>
#include <QDebug>
#include <QsLog.h>

CaptureDevice::CaptureDevice(CaptureDeviceType type,QString name,QString description){
    mType = type;
    mName = name;
    mDescription = description;

}

CaptureDeviceType CaptureDevice::getType(){
    return mType;
}
QString CaptureDevice::getName(){
    return mName;
}
QString CaptureDevice::getDescription(){
    return mDescription;
}
CaptureAssistant::CaptureAssistant(QObject *parent): QObject(parent)
{

}
CaptureAssistant::~CaptureAssistant(){
    this->stop();
}

int CaptureAssistant::start(CaptureDevice* videoDevice,CaptureDevice* audioDevice){
    mVideoDevice = videoDevice;
    mAudioDevice = audioDevice;
    int fps = 20;
    int bitrate = 2000000;
    int width = 1920;
    int height = 1080;
    bool hasVideo = false;
//    const char *videoCodecName = "h264_qsv";//qsv
    const char *videoCodecName = "libx264";//cpu
//    const char *videoCodecName = "h264_nvenc";//nvidia
//    const char *videoCodecName = "h264_amf";//amd

    bool hasAudio = false;
    const char* audioCodecName = "aac";

    switch (videoDevice->getType()) {
        case CAPTURE_VIDEO_SCREEN:
            hasVideo = true;
            break;
        case CAPTURE_VIDEO_CAMERA:
            hasVideo = true;
            break;
        default:
            break;
    }
    switch (audioDevice->getType()) {
        case CAPTURE_AUDIO_SOUNDCARD:
            hasAudio = true;
            break;
        case CAPTURE_AUDIO_MICROPHONE:
            hasAudio = true;
            break;
        case CAPTURE_AUDIO_SOUNDCARD_MICROPHONE:
            hasAudio = true;
            break;
        default:
            break;
    }

    if(hasVideo && hasAudio){
        //TODO 如果同时选中录制视频和音频，则不录制音频，同步功能还在开发中
        hasAudio = false;
    }

    QString url = QString("%1/%2.mp4").arg(SingletonUtils::getInstance()->getRecordDir()).
            arg(QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd-hh-mm-ss"));


    mEncoder = new CaptureFFmpegEncoder(this);

    if(0 != mEncoder->init(hasVideo,videoCodecName, width, height, fps, bitrate,
                   hasAudio,audioCodecName,
                   url.toStdString().data())){
        return -1;
    }

    mEncoder->start();


    if(hasVideo){
        mCaptureVideo = new CaptureVideo(this,mEncoder);
        mCaptureVideo->start();
    }

    if(hasAudio){
        mCaptureAudio = new CaptureAudio(this,mEncoder);
        mCaptureAudio->start();
    }
    return 0;
}
int CaptureAssistant::pause(){
    return 0;
}
int CaptureAssistant::stop(){

    if(mCaptureVideo){
        mCaptureVideo->stop();
        delete mCaptureVideo;
        mCaptureVideo = nullptr;
    }
    if(mCaptureAudio){
        mCaptureAudio->stop();
        delete mCaptureAudio;
        mCaptureAudio = nullptr;
    }
    if(mEncoder){
        mEncoder->stop();
        delete mEncoder;
        mEncoder = nullptr;
    }
    QImage image(1920,1080,QImage::Format_RGB32);
    image.fill(QColor(25,27,38));
    emit setImage(image.copy());
//    std::thread([](CaptureAssistant *assistant){}, this).detach();
    return 0;
}

CaptureDevice* CaptureAssistant::getVideoDevice(){
    return mVideoDevice;
}
CaptureDevice* CaptureAssistant::getAudioDevice(){
    return mAudioDevice;
}
