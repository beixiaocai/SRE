#include "CaptureAudio.h"
#include "FFmpegEncoder.h"
#include <QDebug>
#include <QsLog.h>
#include "Utils.h"


CaptureAudio::CaptureAudio(QObject *parent,FFmpegEncoder* encoder)
    : QObject(parent),mEncoder(encoder)
{
    qDebug()<<"CaptureAudio::CaptureAudio()";
}
CaptureAudio::~CaptureAudio(){
    qDebug()<<"CaptureAudio::~CaptureAudio()";

}

void CaptureAudio::run_thread(void* arg){
    CaptureAudio *capture = (CaptureAudio*)arg;
    qDebug()<<"CaptureAudio::run_thread() mIsStop="<<capture->mIsStop;

}


int CaptureAudio::start(){
    mIsStop = false;
    mThread = new std::thread(CaptureAudio::run_thread, this);


    return 0;
}
int CaptureAudio::pause(){
    return 0;
}
int CaptureAudio::stop(){
    qDebug()<<"CaptureAudio::stop()";
    mIsStop = true;

    mThread->join();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    delete mThread;
    mThread = nullptr;

    return 0;
}

