#include "CaptureVideo.h"
#include "WindowsDXGIDuplication.h"
#include "Frame.h"
#include "FFmpegEncoder.h"
#include "Utils.h"
#include "Assistant.h"
#include <QImage>
#include <QDebug>
#include <QsLog.h>

CaptureVideo::CaptureVideo(QObject *parent,FFmpegEncoder* encoder)
    : QObject(parent),mEncoder(encoder)
{
    mAssist = (Assistant *)parent;
    qDebug()<<"CaptureVideo::CaptureVideo()";
}
CaptureVideo::~CaptureVideo(){
    qDebug()<<"CaptureVideo::~CaptureVideo()";

}

void CaptureVideo::run_thread(void* arg){
    CaptureVideo *capture = (CaptureVideo*)arg;

    WindowsDXGIDuplication dxgiDuplicaion;
    if (!dxgiDuplicaion.InitDevice())
    {
        qDebug()<<"InitDevice error";
        return;
    }
    if (!dxgiDuplicaion.InitDuplication(0))
    {
        qDebug()<<"InitDuplication error";
        return;
    }

    int width = 1920;
    int height = 1080;

    int bufferSize = height * width *4;
    uint8_t* buffer = new uint8_t[bufferSize];

    int64_t frameCount = 0;

    int64_t t1,t2;
    int interval_duration = int(1000/capture->mEncoder->get_fps());//单帧间隔时长（单位毫秒）
    int interval_sleep;//单帧休眠时长（单位毫秒）
    int total_latency_sleep = 0;//积累滞后的补偿休眠时长（单位毫秒）

    while (true)
    {
        t1 = Utils::getCurTimestamp();
        if(capture->mIsStop){
            break;
        }
        if (dxgiDuplicaion.getFrame(10))
        {
            if (dxgiDuplicaion.copyFrameToBuffer(buffer,bufferSize, width, height))
            {
                dxgiDuplicaion.DoneWithFrame();

                Frame *frame = new Frame(FrameFmt::Frame_Fmt_BGRA,bufferSize,width,height,frameCount);
                memcpy_s(frame->data,bufferSize,buffer,bufferSize);
                capture->mEncoder->push_frame(frame);

                if(0==frame->count %2){
                    /*
                    // (bgra->rgba)小端模式，RGBA中R存储在低位，A存储在高位
                    uint8_t *rgba = frame->data;
                    for (int i = 0; i < frame->width * frame->height; i++) {
                        rbga[i] = (rgba[i] & 0xFF000000) |         // ______AA
                              ((rgba[i] & 0x00FF0000) >> 16) | // RR______
                              (rgba[i] & 0x0000FF00) |         // __GG____
                              ((rgba[i] & 0x000000FF) << 16);  // ____BB__
                    }
                    **/
                    // (bgra->rgba)大端模式，RGBA中R存储在高位，A存储在低位

                    uint8_t *rgba = frame->data;
                    for (int i = 0; i < frame->width * frame->height; i++) {
                        rgba[i] = (rgba[i] & 0x000000FF) |         // ______AA
                              ((rgba[i] & 0x0000FF00) << 16) | // RR______
                              (rgba[i] & 0x00FF0000) |         // __GG____
                              ((rgba[i] & 0xFF000000) >> 16);  // ____BB__
                    }
                    QImage image(rgba, frame->width,frame->height,QImage::Format_RGB32);
                    emit capture->mAssist->setImage(image.copy());
                }

                frameCount++;
            }
            else
            {
                qDebug()<<"copyFrameToBuffer error";
            }
        }
        else
        {
            Frame *frame = new Frame(FrameFmt::Frame_Fmt_BGRA,bufferSize,width,height,frameCount);
            memcpy_s(frame->data,bufferSize,buffer,bufferSize);
            capture->mEncoder->push_frame(frame);

            if(0==frame->count %10){
                uint8_t *rgba = frame->data;
                for (int i = 0; i < frame->width * frame->height; i++) {
                    rgba[i] = (rgba[i] & 0x000000FF) |         // ______AA
                          ((rgba[i] & 0x0000FF00) << 16) | // RR______
                          (rgba[i] & 0x00FF0000) |         // __GG____
                          ((rgba[i] & 0xFF000000) >> 16);  // ____BB__
                }
                QImage image(rgba, frame->width,frame->height,QImage::Format_RGB32);
                emit capture->mAssist->setImage(image.copy());
            }


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


int CaptureVideo::start(){
    qDebug()<<"CaptureVideo::start()";
    mIsStop = false;
    mThread = new std::thread(CaptureVideo::run_thread, this);


    return 0;
}
int CaptureVideo::pause(){
    return 0;
}
int CaptureVideo::stop(){
    qDebug()<<"CaptureVideo::stop()";
    mIsStop = true;

    mThread->join();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    delete mThread;
    mThread = nullptr;

    return 0;
}

