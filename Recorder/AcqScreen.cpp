#include "AcqScreen.h"
#include "Duplication.h"
#include "Frame.h"
#include "FFmpegEncoder.h"
#include "Utils.h"
#include <QDebug>
#include <QsLog.h>

AcqScreen::AcqScreen(QObject *parent,FFmpegEncoder* encoder)
    : QObject(parent),mEncoder(encoder)
{
    qDebug()<<"AcqScreen::AcqScreen()";
}
AcqScreen::~AcqScreen(){
    qDebug()<<"AcqScreen::~AcqScreen()";

}

void AcqScreen::run_thread(void* arg){
    AcqScreen *acq = (AcqScreen*)arg;

    Duplication dup;
    if (!dup.InitDevice())
    {
        qDebug()<<"InitDevice error";
        return;
    }
    if (!dup.InitDupl(0))
    {
        qDebug()<<"InitDupl error";
        return;
    }

    int width = 1920;
    int height = 1080;

    int bufferSize = height * width *4;
    uint8_t* buffer = new uint8_t[bufferSize];

    int64_t frameCount = 0;

    int64_t t1,t2;
    int interval_duration = int(1000/acq->mEncoder->get_fps());//单帧间隔时长（单位毫秒）
    int interval_sleep;//单帧休眠时长（单位毫秒）
    int total_latency_sleep = 0;//积累滞后的补偿休眠时长（单位毫秒）

    while (true)
    {
        t1 = Utils::getCurTimestamp();
        if(acq->mIsStop){
            break;
        }
        if (dup.GetFrame())
        {
            if (dup.copyFrameDataToBuffer(buffer,bufferSize, width, height))
            {

                dup.DoneWithFrame();

                Frame *frame = new Frame(FrameFmt::Frame_Fmt_BGRA,bufferSize,width,height,frameCount);
                memcpy_s(frame->data,bufferSize,buffer,bufferSize);
                acq->mEncoder->push_frame(frame);

                frameCount++;
            }
            else
            {
                qDebug()<<"copyFrameDataToBuffer error";
            }
        }
        else
        {
            Frame *frame = new Frame(FrameFmt::Frame_Fmt_BGRA,bufferSize,width,height,frameCount);
            memcpy_s(frame->data,bufferSize,buffer,bufferSize);
            acq->mEncoder->push_frame(frame);


            frameCount++;
        }

        // sleep start
        t2 = Utils::getCurTimestamp();
        interval_sleep = interval_duration - (t2 - t1);

        if(interval_sleep > 0){
            if(total_latency_sleep > 0){
                if(total_latency_sleep >= interval_sleep){
                    total_latency_sleep -= interval_sleep;
                }else{
                    interval_sleep -= total_latency_sleep;
                    total_latency_sleep = 0;
                    std::this_thread::sleep_for(std::chrono::milliseconds(interval_sleep));
                }
            }else{
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_sleep));
            }
        }else if(interval_sleep < 0 ){
            total_latency_sleep += (-interval_sleep);
        }
        // sleep end
    }

    if(buffer){
        delete[] buffer;
        buffer = nullptr;
    }


}


int AcqScreen::start(){
    qDebug()<<"AcqScreen::start()";
    mIsStop = false;
    mThread = new std::thread(AcqScreen::run_thread, this);


    return 0;
}
int AcqScreen::pause(){
    return 0;
}
int AcqScreen::stop(){
    qDebug()<<"AcqScreen::stop()";
    mIsStop = true;

    mThread->join();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    delete mThread;
    mThread = nullptr;

    return 0;
}

