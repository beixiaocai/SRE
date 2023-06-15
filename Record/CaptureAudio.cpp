#include "CaptureAudio.h"
#include "CaptureFFmpegEncoder.h"
#include "CaptureAssistant.h"
#include <AudioRecorder.h>
#include <QDebug>
#include <QsLog.h>


using namespace BXC_AudioRecorderLibrary;

CaptureAudio::CaptureAudio(QObject *parent,CaptureFFmpegEncoder* encoder)
    : QObject(parent),mEncoder(encoder)
{
    mAssistant = (CaptureAssistant *)parent;
    qDebug()<<"CaptureAudio::CaptureAudio()";
}
CaptureAudio::~CaptureAudio(){
    qDebug()<<"CaptureAudio::~CaptureAudio()";

}

void CaptureAudio::run_thread(void* arg){
    CaptureAudio *captureAudio = (CaptureAudio*)arg;
    CaptureDevice *captureDevice = captureAudio->mAssistant->getAudioDevice();

    BXC_RecordDeviceType type = BXC_SOUNDCARD;
    if(CAPTURE_AUDIO_SOUNDCARD == captureDevice->getType()){
        type = BXC_SOUNDCARD;
    }else if(CAPTURE_AUDIO_MICROPHONE == captureDevice->getType()){
        type = BXC_MICROPHONE;
    }
    BXC_AudioRecorder_New(type);
    int frame_time = 1000 * BXC_get_nb_samples() / BXC_get_sample_rate();

    qDebug()<<"CaptureAudio device="<<captureDevice->getDescription()
            <<"，采样声道数="<<BXC_get_nb_channels()
            <<"，采样声道格式="<<BXC_get_nb_bits_sample()
            <<"，采样率="<<BXC_get_sample_rate()
            <<"，单声道一帧音频的采样点数量="<<BXC_get_nb_samples()
            <<"，采样一帧音频耗时="<<frame_time;

    unsigned char *buffer =new unsigned char[8000];
    int size = 0;
    uint64_t count = 0;
    bool ret;
    while (true)
    {
        if(captureAudio->mIsStop){
            break;
        }
        memset(buffer, 0, 8000);
        ret = BXC_get_sample(buffer, size);
        if (ret) {
            if (size > 0) {
//                qDebug()<<"count="<<count<<",size="<<size;
                captureAudio->mEncoder->pushAudioSample(buffer, size);
            }
        }
        else {
            qDebug()<<"BXC_AudioRecorder_reInit";
            BXC_AudioRecorder_reInit();
        }

        ++count;
    }
    delete []buffer;
    buffer = nullptr;

    BXC_AudioRecorder_Destory();

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

