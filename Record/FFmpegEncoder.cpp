#include "FFmpegEncoder.h"
#include "Frame.h"
#include "Utils.h"
#include <QDebug>
#include <QsLog.h>

FFmpegEncoder::FFmpegEncoder(QObject *parent)
    : QObject(parent)
{
    qDebug()<<"FFmpegEncoder::FFmpegEncoder()";
}

FFmpegEncoder::~FFmpegEncoder(){
    qDebug()<<"FFmpegEncoder::~FFmpegEncoder()";
    if(oVideoCodecCtx){
        avcodec_close(oVideoCodecCtx);
        avcodec_free_context(&oVideoCodecCtx);
    }
    if(oFrame_yuv420p){
        av_freep(&oFrame_yuv420p->data[0]);
        av_frame_free(&oFrame_yuv420p);
    }
    if(oFmtCtx){
        if (oFmtCtx && !(oFmtCtx->oformat->flags & AVFMT_NOFILE)) {
            avio_close(oFmtCtx->pb);
        }

        avformat_free_context(oFmtCtx);
    }



}

int FFmpegEncoder::init(const char *codec_name, int width, int height, int fps, int bitrate, const char* url) {
    mFps = fps;

    int ret;
    const AVCodec *videoCodec = avcodec_find_encoder_by_name(codec_name);

    if (!videoCodec) {
        qDebug()<<"avcodec_find_encoder_by_name error";
        return -1;
    }
    oVideoCodecCtx = avcodec_alloc_context3(videoCodec);
    if (!oVideoCodecCtx) {
        qDebug()<<"avcodec_alloc_context3 error";
        return -1;
    }

    // CBR: 固定比特率
//    oVideoCodecCtx->flags |= AV_CODEC_FLAG_QSCALE;
//    oVideoCodecCtx->bit_rate = bitrate;
//    oVideoCodecCtx->rc_min_rate = bitrate;
//    oVideoCodecCtx->rc_max_rate = bitrate;
//    oVideoCodecCtx->bit_rate_tolerance = bitrate;

    //VBR
    oVideoCodecCtx->flags |= AV_CODEC_FLAG_QSCALE;
    oVideoCodecCtx->rc_min_rate = bitrate / 2;
    oVideoCodecCtx->rc_max_rate = bitrate / 2 + bitrate;
    oVideoCodecCtx->bit_rate = bitrate;

    //ABR: 平均码率
    //oVideoCodecCtx->bit_rate = bitrate;

//    oVideoCodecCtx->pix_fmt = AV_PIX_FMT_NV12;
    oVideoCodecCtx->flags |= AV_CODEC_FLAG_QSCALE;
    oVideoCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    oVideoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    oVideoCodecCtx->time_base = {1, fps};
    oVideoCodecCtx->framerate = {fps, 1};
    oVideoCodecCtx->width = width;
    oVideoCodecCtx->height = height;
//    oVideoCodecCtx->keyint_min = fps;
//    oVideoCodecCtx->thread_count = 3;
//    oVideoCodecCtx->max_b_frames = 5;

    if (oVideoCodecCtx->codec_id == AV_CODEC_ID_H264){
        av_opt_set(oVideoCodecCtx->priv_data, "profile", "main", 0);
        av_opt_set(oVideoCodecCtx->priv_data, "b-pyramid", "none", 0);
        av_opt_set(oVideoCodecCtx->priv_data, "preset", "superfast", 0);
        av_opt_set(oVideoCodecCtx->priv_data, "tune", "zerolatency", 0);
    }
    //常用像素格式 AV_PIX_FMT_NV12，AV_PIX_FMT_YUV420P

    // profile 主要调节画质 baseline,main,extended,high

    // preset 主要调节编码速度和质量的平衡，有ultrafast、superfast、veryfast、faster、fast、medium、slow、slower、veryslow、placebo  10个选项，从快到慢

    // tune 主要配合视频类型和视觉优化的参数，或特别的情况。
    //	film： 电影，真人类型；
    //	animation： 动画；
    //	grain： 需要保留大量的grain时用；
    //	stillimage： 静态图像编码时使用；
    //	psnr： 为提高psnr做了优化的参数；
    //	ssim： 为提高ssim做了优化的参数；
    //	fastdecode： 可以快速解码的参数；
    //	zerolatency：零延迟，用在需要非常低的延迟的情况下，比如电视电话会议的编码。不用等待缓冲区填满，即时编码，但非CPU编码会报错

    if (avcodec_open2(oVideoCodecCtx, videoCodec, nullptr) < 0) {
        qDebug()<<"avcodec_open2 error";
        return -1;
    }
    swsCtx_bgra2yuv420p = sws_getCachedContext(swsCtx_bgra2yuv420p,
        oVideoCodecCtx->width, oVideoCodecCtx->height, AV_PIX_FMT_BGRA,
        oVideoCodecCtx->width, oVideoCodecCtx->height, AV_PIX_FMT_YUV420P,
        0, 0, 0, 0);


    //初始化 FmtCtx

    ret = avformat_alloc_output_context2(&oFmtCtx, nullptr, nullptr, url);
    if (ret < 0){
        qDebug()<<"avformat_alloc_output_context2 error";
        return -1;
    }

//    av_opt_set(oVideoCodecCtx->priv_data, "crf", "18", AV_OPT_SEARCH_CHILDREN);


    oVideoStream = avformat_new_stream(oFmtCtx, oVideoCodecCtx->codec);
    if (!oVideoStream)
    {
        qDebug()<<"avformat_new_stream error";
        return -1;
    }

    avcodec_parameters_from_context(oVideoStream->codecpar, oVideoCodecCtx);
    if (!(oFmtCtx->oformat->flags & AVFMT_NOFILE)){
        if (avio_open(&oFmtCtx->pb, url, AVIO_FLAG_WRITE) < 0){
            qDebug()<<"avio_open error";
            return -1;
        }
    }

    oVideoCodecCtx->codec_tag = 0;

    if (oFmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        oVideoCodecCtx->flags = oVideoCodecCtx->flags | AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    oFrame_yuv420p = av_frame_alloc();
    if (!oFrame_yuv420p) {
        qDebug()<<"av_frame_alloc error";
        return -1;
    }
    oFrame_yuv420p->format = oVideoCodecCtx->pix_fmt;
    oFrame_yuv420p->width = oVideoCodecCtx->width;
    oFrame_yuv420p->height =oVideoCodecCtx->height;

    int buff_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,oFrame_yuv420p->width,oFrame_yuv420p->height, 1);
    auto *buff_u = (uint8_t *)av_malloc(buff_size);

    av_image_fill_arrays(oFrame_yuv420p->data,oFrame_yuv420p->linesize,buff_u,AV_PIX_FMT_YUV420P,oFrame_yuv420p->width,oFrame_yuv420p->height,1);

    return 0;
}

void FFmpegEncoder::run_thread(void* arg){
    FFmpegEncoder *encoder = (FFmpegEncoder*)arg;


    //写文件头
    AVDictionary* options = nullptr;
    //av_dict_set(&options, "list_devices", "true", 0);
    int ret = avformat_write_header(encoder->oFmtCtx, &options);
    if (ret < 0){
        qDebug()<<"avformat_write_header error ret="<<ret;
        return;
    }


    int64_t t1,t2;
    int interval_duration = int(1000/encoder->get_fps());//单帧间隔时长（单位毫秒）
    int interval_sleep;//单帧休眠时长（单位毫秒）
    int total_latency_sleep = 0;//积累滞后的补偿休眠时长（单位毫秒）


    while(true){
        t1 = Utils::getCurTimestamp();
        if(encoder->mIsStop){
            if(encoder->mVideoFrameQ.empty()){
                break;
            }

        }

        encoder->mVideoFrameQ_mtx.lock();
        if(!encoder->mVideoFrameQ.empty()){
            Frame *frame = encoder->mVideoFrameQ.front();
            encoder->mVideoFrameQ.pop_front();
            encoder->mVideoFrameQ_mtx.unlock();
            encoder->encode(frame);
            delete frame;
            frame = nullptr;
//            qDebug()<<"FFmpegEncoder::run_thread() interval_sleep="<<interval_sleep<<",total_latency_sleep="<<total_latency_sleep;

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

        }else{
            encoder->mVideoFrameQ_mtx.unlock();
        }


    }

    //写文件尾
    encoder->write_trailer();
}
int FFmpegEncoder::start(){
    qDebug()<<"FFmpegEncoder::start()";

    mIsStop = false;
    mThread = new std::thread(FFmpegEncoder::run_thread, this);


    return 0;
}
int FFmpegEncoder::pause(){
    return 0;
}
int FFmpegEncoder::stop(){
    qDebug()<<"FFmpegEncoder::stop()";
    mIsStop = true;

    mThread->join();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    delete mThread;
    mThread = nullptr;

    return 0;
}

void FFmpegEncoder::encode(Frame* frame) {
    const int in_linesize[1] = { 4 * oVideoCodecCtx->width };

    sws_scale(swsCtx_bgra2yuv420p, (const uint8_t * const *)& frame->data, in_linesize, 0,oVideoCodecCtx->height,
              oFrame_yuv420p->data,
              oFrame_yuv420p->linesize);

    AVPacket pkt;
    av_init_packet(&pkt);

//	if (frame->pts == 1) {
//		frame->key_frame = 1;
//		frame->pict_type = AV_PICTURE_TYPE_I;
//	}
//	else {
//		frame->key_frame = 0;
//		frame->pict_type = AV_PICTURE_TYPE_P;
//	}
    oFrame_yuv420p->pkt_dts = oFrame_yuv420p->pts = av_rescale_q_rnd(frame->count, oVideoCodecCtx->time_base, oVideoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    oFrame_yuv420p->pkt_duration = av_rescale_q_rnd(1, oVideoCodecCtx->time_base, oVideoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    oFrame_yuv420p->pkt_pos = frame->count;


    int ret = avcodec_send_frame(oVideoCodecCtx, oFrame_yuv420p);
    if(ret >= 0){
        ret = avcodec_receive_packet(oVideoCodecCtx, &pkt);
        if (ret >= 0) {
            //success
//                 av_interleaved_write_frame(oFmtCtx, &pkt);
             av_write_frame(oFmtCtx, &pkt);
             av_packet_unref(&pkt);
//             ++frameCount;
        }
        else {
//            qDebug()<<"avcodec_receive_packet error ret="<<ret;
        }
    }else{
        qDebug()<<"avcodec_send_frame error ret="<<ret;
    }

}
void FFmpegEncoder::write_trailer(void) {
    AVPacket pkt;
    av_init_packet(&pkt);
    int ret;
    while (true) {
        ret = avcodec_receive_packet(oVideoCodecCtx, &pkt);
        if (ret >= 0) {
            //success
//             av_interleaved_write_frame(oFmtCtx, &pkt);
             av_write_frame(oFmtCtx, &pkt);
             av_packet_unref(&pkt);
//             ++frameCount;
        }
        else {
//            qDebug()<<"avcodec_receive_packet error ret="<<ret;
            break;
        }

    }
    av_write_trailer(oFmtCtx);

}
int FFmpegEncoder::get_fps(){
    return mFps;
}
void FFmpegEncoder::push_frame(Frame* frame){

    mVideoFrameQ_mtx.lock();
    mVideoFrameQ.push_back(frame);
    mVideoFrameQ_mtx.unlock();
}
