#include "AcqFFmpeg.h"
#include "FFmpegEncoder.h"
#include <QDebug>
#include <QsLog.h>
#include "Utils.h"


AcqFFmpeg::AcqFFmpeg(QObject *parent,FFmpegEncoder* encoder)
    : QObject(parent),mEncoder(encoder)
{
    qDebug()<<"AcqFFmpeg::AcqFFmpeg()";
}
AcqFFmpeg::~AcqFFmpeg(){
    qDebug()<<"AcqFFmpeg::~AcqFFmpeg()";

}

void AcqFFmpeg::run_thread(void* arg){
    AcqFFmpeg *record = (AcqFFmpeg*)arg;
    qDebug()<<"AcqFFmpeg::run_thread() mIsStop="<<record->mIsStop;

}


int AcqFFmpeg::start(){
    mIsStop = false;
    mThread = new std::thread(AcqFFmpeg::run_thread, this);


    return 0;
}
int AcqFFmpeg::pause(){
    return 0;
}
int AcqFFmpeg::stop(){
    qDebug()<<"AcqFFmpeg::stop()";
    mIsStop = true;

    mThread->join();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    delete mThread;
    mThread = nullptr;

    return 0;
}

