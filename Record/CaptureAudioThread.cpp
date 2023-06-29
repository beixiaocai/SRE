#include "CaptureAudioThread.h"
#include "BXC_AudioRecorder.h"
#include "BXC_AvEncoder.h"
#include "FpsControl.h"
#include "Recorder.h"
#include <QDebug>
#include <QsLog.h>

using namespace BXC_MediaLibrary;
CaptureAudioThread::CaptureAudioThread(QObject *parent,CaptureAudioDevice *audioDevice): QThread(parent),mAudioDevice(audioDevice){
    mRecorder = (Recorder *)parent;

}
CaptureAudioThread::~CaptureAudioThread(){
    mIsStop = true;
    while (true) {
        if(isFinished()){
            break;
        }else{
            msleep(1);
        }
    }

}
void CaptureAudioThread::run(){

    mIsStop = false;
    int ret;
    BXC_AudioRecorder * audioRecorder = mRecorder->getAudioRecorder();
    BXC_AvEncoder * avEncoder = mRecorder->getAvEncoder();

    uint8_t* audioBuff = new uint8_t[10000];
    int      audioBuffSize = 0;

    int      recvBuffSize = 0;
    uint8_t* recvBuff = new uint8_t[10000];


    const int frameSize = 4608; // 4224=1056 * 4; 4608=1152 * 4;
    int64_t timestamp = 0;
    int64_t frameTimestamp = 0;
    int64_t frameCount = 0;

    while (true)
    {
        if(mIsStop){
            break;
        }

        ret = BXC_get_sample(audioRecorder, recvBuff, recvBuffSize, timestamp);
        if (ret >= 0) {
            if (recvBuffSize > 0) {
                if (0 == audioBuffSize) {
                    frameTimestamp = timestamp;
                }
                memcpy(audioBuff + audioBuffSize, recvBuff, recvBuffSize);
                audioBuffSize += recvBuffSize;

                if (audioBuffSize >= frameSize) {
                    BXC_send_audio_frame(avEncoder, frameSize, audioBuff, frameTimestamp, frameCount);

                    audioBuffSize -= frameSize;
                    memmove(audioBuff, audioBuff + frameSize, audioBuffSize);
                    frameTimestamp = timestamp;
                    ++frameCount;
                }
            }
        }else {

        }
    }

    if (recvBuff) {
        delete[]recvBuff;
        recvBuff = nullptr;
    }
    if (audioBuff) {
        delete[]audioBuff;
        audioBuff = nullptr;
    }
    this->exit();
}
