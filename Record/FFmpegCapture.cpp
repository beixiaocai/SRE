#include "FFmpegCapture.h"
#include <QDebug>
#include <QsLog.h>

FFmpegCapture::FFmpegCapture(QObject *parent): QObject(parent)
{

}
FFmpegCapture::~FFmpegCapture()
{

}
int FFmpegCapture::init(){

    //av_register_all();
//    avdevice_register_all();

    std::string deviceName = "video=imx188_azurewave(p)";
    int mediaType = AVMEDIA_TYPE_VIDEO;

//    deviceName = "audio=Microphone (Realtek(R) Audio)";
//    int mediaType = AVMEDIA_TYPE_AUDIO;


    qDebug()<<"deviceName="<<deviceName<<",mediaType="<<mediaType;

    const AVInputFormat* inputFmt = av_find_input_format("dshow");
    AVFormatContext* fmtCtx = nullptr;
    AVDictionary* options = nullptr;
    //av_dict_set(&options, "list_devices", "true", 0);


    //打开输入设备
    if (0 != avformat_open_input(&fmtCtx, deviceName.data(), inputFmt, &options)) {
        qDebug()<<"avformat_open_input error";
        //关闭设备
        avformat_close_input(&fmtCtx);

        return -1;
    }
    if (0 != avformat_find_stream_info(fmtCtx, nullptr)) {
        qDebug()<<"avformat_find_stream_info error";
        return -1;
    }
    AVStream *audioStream = nullptr;
    for (int i = 0; i < fmtCtx->nb_streams; i++) {
        if (fmtCtx->streams[i]->codecpar->codec_type == mediaType) {
            audioStream = fmtCtx->streams[i];
            break;
        }
    }
    const AVCodec* audioCodec = avcodec_find_decoder(audioStream->codecpar->codec_id);
    AVCodecContext * audioCodecCtx = avcodec_alloc_context3(audioCodec);
    avcodec_parameters_to_context(audioCodecCtx, audioStream->codecpar);


    AVPacket *pkt = av_packet_alloc();
    int ret;
    while (true) {
        ret = av_read_frame(fmtCtx, pkt);

        if (0 == ret) {

        }
        else {
            qDebug()<<"av_read_frame error";
            break;
        }
    }

    return 0;
}
