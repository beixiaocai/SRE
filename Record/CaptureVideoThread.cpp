#include "CaptureVideoThread.h"
#include "BXC_VideoRecorder.h"
#include "BXC_AvEncoder.h"
#include "FpsControl.h"
#include "Recorder.h"
#include <QDebug>
#include <QsLog.h>

using namespace BXC_MediaLibrary;
CaptureVideoThread::CaptureVideoThread(QObject *parent,CaptureVideoDevice *videoDevice): QThread(parent),mVideoDevice(videoDevice){
    mRecorder = (Recorder *)parent;

}
CaptureVideoThread::~CaptureVideoThread(){
    mIsStop = true;
    while (true) {
        if(isFinished()){
            break;
        }else{
            msleep(1);
        }
    }
    //    terminate();//强制结束
    //    wait();//配合terminate()强制结束后，等待回收资源
}
void CaptureVideoThread::run(){
    mIsStop = false;

    int fps = mVideoDevice->getFps();
    int width = mVideoDevice->width;
    int height = mVideoDevice->height;

    int      frameBuffSize = height * width * 4;
    uint8_t* frameBuff = new uint8_t[frameBuffSize];
    uint8_t* frameBuff_rgba = new uint8_t[frameBuffSize];

    int64_t frameTimestamp = 0;
    int64_t frameCount = 0;

    int show_interval = 5;//屏幕显示时间隔数量
    if(mVideoDevice->isCamera()){
        show_interval = 2;
    }

    int ret;
    FpsControl fpsControl(fps);
    fpsControl.realTimeStart();

    while (true)
    {
        fpsControl.intervalStart();
        if(mIsStop){
            break;
        }
        ret = BXC_get_frame(mRecorder->getVideoRecorder(), frameBuff, frameBuffSize,frameTimestamp);

        if (ret >= 0) {
            BXC_send_video_frame(mRecorder->getAvEncoder(),frameBuffSize,frameBuff,frameTimestamp,
                               width,height,frameCount);

            if(0 == frameCount % show_interval){
                if("BGRA" == mVideoDevice->pixelFormat){
                    // (bgra->rgba)大端模式，RGBA中R存储在高位，A存储在低位
                    memcpy_s(frameBuff_rgba,frameBuffSize,frameBuff,frameBuffSize);
                    for (int i = 0; i < width*height; i++) {
                        frameBuff_rgba[i] = (frameBuff_rgba[i] & 0x000000FF) |         // ______AA
                              ((frameBuff_rgba[i] & 0x0000FF00) << 16) | // RR______
                              (frameBuff_rgba[i] & 0x00FF0000) |         // __GG____
                              ((frameBuff_rgba[i] & 0xFF000000) >> 16);  // ____BB__
                    }
                    QImage image(frameBuff_rgba, width,height,QImage::Format_RGB32);
                    emit mRecorder->setImage(image.copy());

                }else if("RGB" == mVideoDevice->pixelFormat){
                    QImage image(frameBuff, width,height,QImage::Format_RGB888);
                    emit mRecorder->setImage(image.copy());
                }
            }
            ++frameCount;
            fpsControl.realTimeIncrease();
        }else{
            continue;
            //break;
        }

        fpsControl.adjust();
//        qDebug() <<"realTimeFps=" << fpsControl.getRealTimeFps();
    }

    if(frameBuff){
        delete[] frameBuff;
        frameBuff = nullptr;
    }
    if(frameBuff_rgba){
        delete[] frameBuff_rgba;
        frameBuff_rgba = nullptr;
    }
    this->exit();
}
