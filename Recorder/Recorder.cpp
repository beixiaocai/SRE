#include "Recorder.h"
#include "Duplication.h"
#include "Frame.h"
#include "FFmpegEncoder.h"
#include "AcqScreen.h"
#include "AcqFFmpeg.h"
#include <QDebug>
#include <QsLog.h>
#include <thread>
#include "Utils.h"
#include "PreviewPlayer.h"

#define FFMPEG_ENC_264_QSV "h264_qsv"
#define FFMPEG_ENC_264_CPU "libx264"
#define FFMPEG_ENC_264_NVIDIA "h264_nvenc"
#define FFMPEG_ENC_264_AMD "h264_amf"

Recorder::Recorder(QObject *parent,PreviewPlayer *previewPlayer)
    : QObject(parent),mPreviewPlayer(previewPlayer)
{

}
Recorder::~Recorder(){

}


int Recorder::start(){

    int fps = 20;
    int bitrate = 2000000;
    int width = 1920;
    int height = 1080;

    QString url = QString("%1.mp4").arg(QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd-hh-mm-ss"));

    mEncoder = new FFmpegEncoder(this,mPreviewPlayer);
    mEncoder->init(FFMPEG_ENC_264_CPU, width, height, fps, bitrate,url.toStdString().data());
    mEncoder->start();

    mAcqScreen = new AcqScreen(this,mEncoder);
    mAcqScreen->start();


    return 0;
}
int Recorder::pause(){
    return 0;
}
int Recorder::stop(){

    if(mAcqScreen){
        mAcqScreen->stop();
        delete mAcqScreen;
        mAcqScreen = nullptr;
    }
    if(mEncoder){
        mEncoder->stop();
        delete mEncoder;
        mEncoder = nullptr;
    }
    /*
    std::thread([](Recorder *recorder){

        if(recorder->mAcqScreen){
            recorder->mAcqScreen->stop();
            delete recorder->mAcqScreen;
            recorder->mAcqScreen = nullptr;
        }
        if(recorder->mEncoder){
            recorder->mEncoder->stop();
            delete recorder->mEncoder;
            recorder->mEncoder = nullptr;
        }

    }, this).detach();
    */

    return 0;
}
