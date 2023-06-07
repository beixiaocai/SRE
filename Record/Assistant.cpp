#include "Assistant.h"
#include "WindowsDXGIDuplication.h"
#include "Frame.h"
#include "FFmpegEncoder.h"
#include "CaptureVideo.h"
#include "CaptureAudio.h"
#include <thread>
#include "Utils.h"
#include "SingletonUtils.h"
#include <QDebug>
#include <QsLog.h>

Assistant::Assistant(QObject *parent)
    : QObject(parent)
{

}
Assistant::~Assistant(){

}


int Assistant::start(){

    int fps = 20;
    int bitrate = 2000000;
    int width = 1920;
    int height = 1080;
//    const char *codec_name = "h264_qsv";//qsv
    const char *codec_name = "libx264";//cpu
//    const char *codec_name = "h264_nvenc";//nvidia
//    const char *codec_name = "h264_amf";//amd

    QString url = QString("%1/%2.mp4").arg(SingletonUtils::getInstance()->getRecordDir()).
            arg(QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd-hh-mm-ss"));

    mEncoder = new FFmpegEncoder(this);
    mEncoder->init(codec_name, width, height, fps, bitrate,url.toStdString().data());
    mEncoder->start();

    mCaptureVideo = new CaptureVideo(this,mEncoder);
    mCaptureVideo->start();


    return 0;
}
int Assistant::pause(){
    return 0;
}
int Assistant::stop(){

    if(mCaptureVideo){
        mCaptureVideo->stop();
        delete mCaptureVideo;
        mCaptureVideo = nullptr;
    }
    if(mEncoder){
        mEncoder->stop();
        delete mEncoder;
        mEncoder = nullptr;
    }
    /*
    std::thread([](Recorder *recorder){


    }, this).detach();
    */

    return 0;
}
