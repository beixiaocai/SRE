#include "CaptureFFmpegEncoder.h"
#include "Utils.h"
#include <QDebug>
#include <QsLog.h>
extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libswresample/swresample.h>

}
#define AUDIO_SAMPLEBUFFER_MAX_SIZE 1048576 /* 1048576=1*1024*1024=1mb*/

CaptureFFmpegEncoder::CaptureFFmpegEncoder(QObject *parent)
    : QObject(parent)
{
    qDebug()<<"CaptureFFmpegEncoder::CaptureFFmpegEncoder()";

    mAudioSampleBuffer = new unsigned char[AUDIO_SAMPLEBUFFER_MAX_SIZE];
    mAudioSampleBufferSize = 0;

}

CaptureFFmpegEncoder::~CaptureFFmpegEncoder(){
    qDebug()<<"CaptureFFmpegEncoder::~CaptureFFmpegEncoder()";
    this->release();
}

int CaptureFFmpegEncoder::init(bool hasVideo,const char* videoCodecName,int width, int height, int fps, int bitrate, bool hasAudio,const char* audioCodecName,const char* url) {
    mHasVideo = hasVideo;
    mHasAudio = hasAudio;


    if(hasVideo){

        const AVCodec *videoCodec = avcodec_find_encoder_by_name(videoCodecName);
        if (!videoCodec) {
            qDebug()<<"avcodec_find_encoder_by_name error";
            return -1;
        }
        mVideoFps = fps;

        mVideoCodecCtx = avcodec_alloc_context3(videoCodec);
        if (!mVideoCodecCtx) {
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
        mVideoCodecCtx->flags |= AV_CODEC_FLAG_QSCALE;
        mVideoCodecCtx->rc_min_rate = bitrate / 2;
        mVideoCodecCtx->rc_max_rate = bitrate / 2 + bitrate;
        mVideoCodecCtx->bit_rate = bitrate;
        //ABR: 平均码率
        //oVideoCodecCtx->bit_rate = bitrate;

        mVideoCodecCtx->flags |= AV_CODEC_FLAG_QSCALE;
        mVideoCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;// AV_PIX_FMT_NV12，AV_PIX_FMT_YUV420P
        mVideoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
        mVideoCodecCtx->time_base = {1, fps};
        mVideoCodecCtx->framerate = {fps, 1};
        mVideoCodecCtx->width = width;
        mVideoCodecCtx->height = height;
        //    mVideoCodecCtx->keyint_min = fps;
        //    mVideoCodecCtx->thread_count = 3;
        //    mVideoCodecCtx->max_b_frames = 5;

        if (AV_CODEC_ID_H264 == mVideoCodecCtx->codec_id){
            av_opt_set(mVideoCodecCtx->priv_data, "profile", "main", 0);
            av_opt_set(mVideoCodecCtx->priv_data, "b-pyramid", "none", 0);
            av_opt_set(mVideoCodecCtx->priv_data, "preset", "superfast", 0);
            av_opt_set(mVideoCodecCtx->priv_data, "tune", "zerolatency", 0);

        }else if(AV_CODEC_ID_H265 == mVideoCodecCtx->codec_id){


        }


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

        if (avcodec_open2(mVideoCodecCtx, videoCodec, nullptr) < 0) {// 将编码器上下文和编码器进行关联
            qDebug()<<"avcodec_open2 error";
            return -1;
        }

        mVideoSwsCtx_bgra2yuv420p = sws_getCachedContext(mVideoSwsCtx_bgra2yuv420p,
                                                         mVideoCodecCtx->width, mVideoCodecCtx->height, AV_PIX_FMT_BGRA,
                                                         mVideoCodecCtx->width, mVideoCodecCtx->height, AV_PIX_FMT_YUV420P,
                                                         0, 0, 0, 0);


    }
    if(hasAudio){

        const AVCodec* audioCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
        if (!audioCodec) {
            qDebug()<<"audio avcodec_find_encoder error";
            return -1;
        }

        int audio_sample_rate;
        int audio_nb_samples;

        if (AV_CODEC_ID_MP3 == audioCodec->id) {
            audio_sample_rate = 48000;
            audio_nb_samples = 1056;
        }
        else if (AV_CODEC_ID_AAC == audioCodec->id) {
            audio_sample_rate = 44100;
            audio_nb_samples = 1024;
        }
        else {
            qDebug()<<"audio unsupported codec, name="<<audioCodec->name;
            return -1;
        }
        this->mAudioFps = audio_sample_rate/audio_nb_samples;
        mAudioCodecCtx = avcodec_alloc_context3(audioCodec);
        if (!mAudioCodecCtx) {
            qDebug()<<"audio avcodec_alloc_context3 error";
            return -1;
        }

        mAudioCodecCtx->codec_id = audioCodec->id;
        mAudioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
        mAudioCodecCtx->bit_rate = 128000;//音频码率
        mAudioCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;// 声道层
        mAudioCodecCtx->channels = av_get_channel_layout_nb_channels(mAudioCodecCtx->channel_layout);// 声道数
        mAudioCodecCtx->sample_rate = audio_sample_rate;//采样率
        mAudioCodecCtx->frame_size = audio_nb_samples;//每帧单个通道的采样点数
        mAudioCodecCtx->profile = FF_PROFILE_AAC_LOW;
        mAudioCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;//ffmpeg对于AAC编码的采样点格式默认只支持AV_SAMPLE_FMT_FLTP，通常PCM文件或者播放器播放的音频采样点格式是 AV_SAMPLE_FMT_S16
        mAudioCodecCtx->time_base = { audio_nb_samples, audio_sample_rate };
        mAudioCodecCtx->framerate = { audio_sample_rate, audio_nb_samples };

        if (avcodec_open2(mAudioCodecCtx, audioCodec, NULL) < 0) {// 将编码器上下文和编码器进行关联
            qDebug()<<"audio avcodec_open2 error";
            return -1;
        }
    }

    if (avformat_alloc_output_context2(&mFmtCtx, nullptr, nullptr, url) < 0){
        qDebug()<<"audio avformat_alloc_output_context2 error";
        return -1;
    }
//    av_dump_format(mFmtCtx, 0, url, 1);

    if(hasVideo){
//        av_opt_set(mVideoCodecCtx->priv_data, "crf", "18", AV_OPT_SEARCH_CHILDREN);
        mVideoStream = avformat_new_stream(mFmtCtx, mVideoCodecCtx->codec);
        if (!mVideoStream)
        {
            qDebug()<<"audio avformat_new_stream error";
            return -1;
        }
        mVideoStream->id = mFmtCtx->nb_streams - 1;
        mVideoIndex = mVideoStream->index;
        avcodec_parameters_from_context(mVideoStream->codecpar, mVideoCodecCtx);
        mFmtCtx->video_codec_id = mFmtCtx->oformat->video_codec;
        if (mFmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
            mVideoCodecCtx->flags = mVideoCodecCtx->flags | AV_CODEC_FLAG_GLOBAL_HEADER;
        }

    }
    if(hasAudio){
        mAudioStream = avformat_new_stream(mFmtCtx, mAudioCodecCtx->codec);
        if (!mAudioStream) {
            qDebug()<<"audio avformat_new_stream error";
            return -1;
        }
        mAudioStream->id = mFmtCtx->nb_streams - 1;
        mAudioIndex = mAudioStream->id;
        avcodec_parameters_from_context(mAudioStream->codecpar, mAudioCodecCtx);
        mFmtCtx->audio_codec_id = mFmtCtx->oformat->audio_codec;
    }

    // public start
    if (!(mFmtCtx->oformat->flags & AVFMT_NOFILE)){
        if (avio_open(&mFmtCtx->pb, url, AVIO_FLAG_WRITE) < 0){
            qDebug()<<"audio avio_open error";
            return -1;
        }
    }
    // public end

    return 0;
}
void CaptureFFmpegEncoder::release(){

    if(mAudioSampleBuffer){
        delete []mAudioSampleBuffer;
        mAudioSampleBuffer = nullptr;
    }

    if(mVideoCodecCtx){
        avcodec_close(mVideoCodecCtx);
        avcodec_free_context(&mVideoCodecCtx);
        mVideoCodecCtx = nullptr;
        mVideoIndex = -1;
    }
    if (mAudioCodecCtx) {
        avcodec_close(mAudioCodecCtx);
        avcodec_free_context(&mAudioCodecCtx);
        mAudioCodecCtx = nullptr;
        mAudioIndex = -1;
    }

    if(mFmtCtx){
        if (mFmtCtx && !(mFmtCtx->oformat->flags & AVFMT_NOFILE)) {
            avio_close(mFmtCtx->pb);
        }

        avformat_free_context(mFmtCtx);
    }

}
void CaptureFFmpegEncoder::run_thread(void* arg){
    CaptureFFmpegEncoder *encoder = (CaptureFFmpegEncoder*)arg;

    AVDictionary* options = nullptr;
    if (avformat_write_header(encoder->mFmtCtx, &options) < 0){//写文件头
        qDebug()<<"avformat_write_header error";
        return;
    }

    if(encoder->mHasVideo && !encoder->mHasAudio){
        encoder->handleVideo();
    }else if(!encoder->mHasVideo && encoder->mHasAudio){
        encoder->handleAudio();
    }else if(encoder->mHasVideo && encoder->mHasAudio){
        encoder->handleVideoAndAudio();
    }

    av_write_trailer(encoder->mFmtCtx);//写文件尾
}
bool CaptureFFmpegEncoder::handleVideo(){

    AVFrame  *frame_yuv420p = av_frame_alloc();
    frame_yuv420p->format = mVideoCodecCtx->pix_fmt;
    frame_yuv420p->width = mVideoCodecCtx->width;
    frame_yuv420p->height = mVideoCodecCtx->height;
    int frame_yuv420p_buff_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,frame_yuv420p->width,frame_yuv420p->height, 1);
    uint8_t *frame_yuv420p_buff = (uint8_t *)av_malloc(frame_yuv420p_buff_size);
    av_image_fill_arrays(frame_yuv420p->data,frame_yuv420p->linesize,frame_yuv420p_buff,AV_PIX_FMT_YUV420P,frame_yuv420p->width,frame_yuv420p->height,1);

    const int videoFrame_srcStride[1] = { 4 * frame_yuv420p->width };
    AVPacket* videoPkt = av_packet_alloc();

    int64_t  videoFrameCount = 0;
    int ret;
    int rc;

    //同步控制 start
    int64_t t1,t2;
    int interval_duration = int(1000/mVideoFps);//单帧间隔时长（单位毫秒）
    int interval_sleep;//单帧休眠时长（单位毫秒）
    int total_latency_sleep = 0;//积累滞后的补偿休眠时长（单位毫秒）
    //同步控制 end
    qDebug()<<"video interval_duration="<<interval_duration<<",mVideoFps="<<mVideoFps;

    while(true){
        t1 = Utils::getCurTimestamp();
        mVideoFrameQ_mtx.lock();
        if(!mVideoFrameQ.empty()){

            VideoFrame *videoFrame = mVideoFrameQ.front();
            mVideoFrameQ.pop_front();
            mVideoFrameQ_mtx.unlock();


            sws_scale(mVideoSwsCtx_bgra2yuv420p, (const uint8_t * const *)& videoFrame->data, videoFrame_srcStride, 0,frame_yuv420p->height,
                      frame_yuv420p->data,frame_yuv420p->linesize);

            frame_yuv420p->pkt_dts = frame_yuv420p->pts = av_rescale_q_rnd(videoFrameCount, mVideoCodecCtx->time_base, mVideoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            frame_yuv420p->pkt_duration = av_rescale_q_rnd(1, mVideoCodecCtx->time_base,mVideoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            frame_yuv420p->pkt_pos = videoFrameCount;


            avcodec_send_frame(mVideoCodecCtx, frame_yuv420p);
            rc = 0;
            while (true)
            {
                ret = avcodec_receive_packet(mVideoCodecCtx, videoPkt);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    //qDebug()<<"video avcodec_receive_packet EOF";
                    break;
                }
                else if (ret < 0) {
                    qDebug()<<"video avcodec_receive_packet ret < 0";
                    break;
                }
                else {
                    //av_interleaved_write_frame(oFmtCtx, &pkt);
                    av_write_frame(mFmtCtx, videoPkt);
                    av_packet_unref(videoPkt);
                    ++videoFrameCount;
                    ++rc;
                }
            }
            if(0==rc){
                ++videoFrameCount;
            }

            delete videoFrame;
            videoFrame = nullptr;


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
            if(mIsStop){
                mVideoFrameQ_mtx.unlock();
                break;
            }else{
                mVideoFrameQ_mtx.unlock();
            }

        }

    }
    while (true) {
        ret = avcodec_receive_packet(mVideoCodecCtx, videoPkt);
        if (ret >= 0) {
            //av_interleaved_write_frame(oFmtCtx, &videoPkt);
            av_write_frame(mFmtCtx, videoPkt);
            av_packet_unref(videoPkt);
            ++videoFrameCount;
        }
        else {
            break;
        }
    }

    if(frame_yuv420p){
        av_freep(&frame_yuv420p->data[0]);
        av_frame_free(&frame_yuv420p);
    }

    return true;
}
bool CaptureFFmpegEncoder::handleAudio(){

    // 音频输入参数start
    uint64_t in_channel_layout = AV_CH_LAYOUT_STEREO;// 输入声道层
    int in_channels = av_get_channel_layout_nb_channels(in_channel_layout);// 输入声道数
    AVSampleFormat in_sample_fmt = AV_SAMPLE_FMT_S16;
    int in_sample_rate = 48000;
    int in_nb_samples = 1152;//1024,1056,1152;

    AVFrame* in_frame = av_frame_alloc();
    in_frame->channels = in_channels;
    in_frame->format = in_sample_fmt;
    in_frame->nb_samples = in_nb_samples;

    int in_frame_buff_size = av_samples_get_buffer_size(NULL, in_channels, in_nb_samples, in_sample_fmt, 1);
    uint8_t* in_frame_buff = (uint8_t*)av_malloc(in_frame_buff_size);
    avcodec_fill_audio_frame(in_frame, in_channels, in_sample_fmt, (const uint8_t*)in_frame_buff, in_frame_buff_size, 1);

    // 音频输入参数end


    // 音频输出参数start
    uint64_t out_channel_layout = mAudioCodecCtx->channel_layout;
    int out_channels = mAudioCodecCtx->channels;// av_get_channel_layout_nb_channels(out_channel_layout);
    AVSampleFormat out_sample_fmt = mAudioCodecCtx->sample_fmt;// AV_SAMPLE_FMT_FLTP;
    int out_sample_rate = mAudioCodecCtx->sample_rate;// 44100;
    int out_nb_samples = mAudioCodecCtx->frame_size;// 1024;

    AVFrame* out_frame = av_frame_alloc();
    out_frame->channels = out_channels;
    out_frame->format = out_sample_fmt;
    out_frame->nb_samples = out_nb_samples;

    int out_frame_buff_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
    uint8_t* out_frame_buff = (uint8_t*)av_malloc(out_frame_buff_size);
    avcodec_fill_audio_frame(out_frame, out_channels, out_sample_fmt, (const uint8_t*)out_frame_buff, out_frame_buff_size, 1);

    //uint8_t** out_convert_data = (uint8_t**)av_calloc(out_channels, sizeof(*out_convert_data));
    //av_samples_alloc(out_convert_data, NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
    AVPacket* out_pkt = av_packet_alloc();
    av_new_packet(out_pkt, out_frame_buff_size);
    out_pkt->data = NULL;
    out_pkt->size = 0;

    // 音频输出参数end
    SwrContext* audioSwrCtx = swr_alloc();
    swr_alloc_set_opts(audioSwrCtx,
                       out_channel_layout,
                       out_sample_fmt,
                       out_sample_rate,
                       in_channel_layout,
                       in_sample_fmt,
                       in_sample_rate,
                       0, NULL);
    swr_init(audioSwrCtx);


    int in_sample_size = in_nb_samples * av_get_bytes_per_sample(in_sample_fmt) * in_channels;//读取一个采样点所有声道的pcm数据
    int out_sample_size = out_nb_samples * av_get_bytes_per_sample(out_sample_fmt);//一个采样点重采样后单声道的pcm数据

//    qDebug()<<"in_frame_buff_size="<<in_frame_buff_size;
//    qDebug()<<"out_frame_buff_size="<<out_frame_buff_size;
//    qDebug()<<"in_sample_size="<<in_sample_size;
//    qDebug()<<"out_sample_size="<<out_sample_size;

    int64_t  audioFrameCount = 0;
    int ret;
    int rc;

    while (true)
    {
        mAudioSampleBuffer_mtx.lock();
        if (mAudioSampleBufferSize > in_sample_size) {

            memcpy(in_frame_buff, mAudioSampleBuffer, in_sample_size);
            mAudioSampleBufferSize -= in_sample_size;
            memmove(mAudioSampleBuffer, mAudioSampleBuffer + in_sample_size, mAudioSampleBufferSize);
            mAudioSampleBuffer_mtx.unlock();

            swr_convert(audioSwrCtx, out_frame->data, out_nb_samples, (const uint8_t**)in_frame->data, in_nb_samples);

            //swr_convert(mAudioSwrCtx, out_convert_data, out_nb_samples, (const uint8_t**)in_frame->data, in_nb_samples);
            //memcpy(out_frame->data[0], out_convert_data[0], out_sample_size);
            //memcpy(out_frame->data[1], out_convert_data[1], out_sample_size);

            out_frame->pts = out_frame->pkt_dts = av_rescale_q_rnd(audioFrameCount,
                                                                   mAudioCodecCtx->time_base,
                                                                   mAudioStream->time_base,
                                                                   (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

            out_frame->pkt_duration = av_rescale_q_rnd(1,
                                                       mAudioCodecCtx->time_base,
                                                       mAudioStream->time_base,
                                                       (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

            out_frame->pkt_pos = audioFrameCount;

            avcodec_send_frame(mAudioCodecCtx, out_frame);
            rc = 0;
            while (true)
            {
                ret = avcodec_receive_packet(mAudioCodecCtx, out_pkt);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                    qDebug()<<"audio avcodec_receive_packet EOF";
                    break;
                }
                else if (ret < 0) {
                    qDebug()<<"audio avcodec_receive_packet ret < 0";
                    break;
                }
                else {
                    out_pkt->stream_index =mAudioStream->index;
                    av_write_frame(mFmtCtx, out_pkt);
                    //av_interleaved_write_frame(encoder->mFmtCtx, out_pkt);
                    av_packet_unref(out_pkt);
                    ++audioFrameCount;
                    ++rc;
                }
            }

            if(0==rc){
                ++audioFrameCount;
            }
            // sleep start
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            // sleep end
        }
        else {
            if(mIsStop){
                mAudioSampleBuffer_mtx.unlock();
                break;
            }else{
                mAudioSampleBuffer_mtx.unlock();
            }
        }
    }

    while (true) {
        ret = avcodec_receive_packet(mAudioCodecCtx, out_pkt);
        if (ret >= 0) {
            //av_interleaved_write_frame(oFmtCtx, &out_pkt);
            av_write_frame(mFmtCtx, out_pkt);
            av_packet_unref(out_pkt);
            ++audioFrameCount;
        }
        else {
            break;
        }
    }

    swr_free(&audioSwrCtx);

    av_free(in_frame_buff);
    av_frame_free(&in_frame);

    av_packet_free(&out_pkt);
    av_free(out_frame_buff);
    av_frame_free(&out_frame);

    //av_freep(&out_convert_data[0]);
    //out_convert_data[0] = NULL;
    //free(out_convert_data);
    //out_convert_data = NULL;

    return true;
}
bool CaptureFFmpegEncoder::handleVideoAndAudio(){
    return true;
}
int CaptureFFmpegEncoder::start(){
    qDebug()<<"CaptureFFmpegEncoder::start()";

    mIsStop = false;
    mThread = new std::thread(CaptureFFmpegEncoder::run_thread, this);


    return 0;
}
int CaptureFFmpegEncoder::pause(){
    return 0;
}
int CaptureFFmpegEncoder::stop(){
    qDebug()<<"CaptureFFmpegEncoder::stop()";
    mIsStop = true;

    mThread->join();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    delete mThread;
    mThread = nullptr;

    return 0;
}
int CaptureFFmpegEncoder::getVideoFps(){
    return mVideoFps;
}
int CaptureFFmpegEncoder::getAudioFps(){
    return mAudioFps;
}
void CaptureFFmpegEncoder::pushVideoFrame(VideoFrame* videoFrame){

    mVideoFrameQ_mtx.lock();
    mVideoFrameQ.push_back(videoFrame);
    mVideoFrameQ_mtx.unlock();
}

void CaptureFFmpegEncoder::pushAudioSample(unsigned char* buffer, int size) {

    mAudioSampleBuffer_mtx.lock();
    if((mAudioSampleBufferSize+size) > AUDIO_SAMPLEBUFFER_MAX_SIZE){
        //超过了音频原始数据的缓存上线
        qDebug()<<"CaptureFFmpegEncoder::pushAudioSample 超过了音频原始数据的缓存上线";
    }else{
        memcpy(mAudioSampleBuffer+ mAudioSampleBufferSize, buffer, size);
        mAudioSampleBufferSize += size;
    }
    mAudioSampleBuffer_mtx.unlock();
}
