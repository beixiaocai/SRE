#ifndef CAPTUREFFMPEGENCODER_H
#define CAPTUREFFMPEGENCODER_H

#include <QObject>
extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}
#include <QQueue>
#include <QMutex>
#include <thread>


class CaptureFFmpegEncoder : public QObject
{
    Q_OBJECT
public:
    enum VideoFrameFmt{
        Fmt_BGRA,
        Fmt_RGB32,
    };
    struct VideoFrame
    {
    public:
        VideoFrame(VideoFrameFmt fmt,int size,int width, int height, int64_t count) {
            this->fmt = fmt;
            this->size = size;
            this->width = width;
            this->height = height;
//            this->count = count;
            this->data = new uint8_t[this->size];
        }
        ~VideoFrame() {
            delete[] this->data;
            this->data = nullptr;
        }
        VideoFrameFmt fmt;
        int size;
        int width;
        int height;
        uint8_t* data;
//        int64_t  count;
    };

    CaptureFFmpegEncoder(QObject *parent);
    ~CaptureFFmpegEncoder();

public:
    int init(bool hasVideo,const char* videoCodecName,int width, int height, int fps, int bitrate, bool hasAudio,const char* audioCodecName,const char* url);

    int start();
    int pause();
    int stop();
    int getVideoFps();//获取视频编码后输出的帧率
    int getAudioFps();//获取音频编码后输出的帧率

    void pushVideoFrame(VideoFrame* videoFrame);
    void pushAudioSample(unsigned char* buffer, int size);

private:
    void release();
    static void run_thread(void* arg);
    bool handleVideo();
    bool handleAudio();
    bool handleVideoAndAudio();
private:
    bool mHasVideo = false;
    bool mHasAudio = false;

    bool mIsRecording = false;//是否在录制中
    bool mIsStop = true;
    std::thread *mThread;//编码线程

    AVFormatContext *mFmtCtx = nullptr;

    //视频
    int mVideoFps;
    AVCodecContext  *mVideoCodecCtx = nullptr;
    AVStream        *mVideoStream = nullptr;
    int              mVideoIndex = -1;
    SwsContext      *mVideoSwsCtx_bgra2yuv420p = nullptr;
    QQueue<VideoFrame*> mVideoFrameQ;
    QMutex              mVideoFrameQ_mtx;
    //音频
    int mAudioFps;
    AVCodecContext* mAudioCodecCtx = nullptr;
    AVStream*       mAudioStream = nullptr;
    int             mAudioIndex = -1;
    unsigned char*  mAudioSampleBuffer = nullptr;
    int             mAudioSampleBufferSize = 0;
    QMutex          mAudioSampleBuffer_mtx;


signals:

private slots:
};

#endif // CAPTUREFFMPEGENCODER_H
