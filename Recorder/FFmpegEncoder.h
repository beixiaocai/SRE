#ifndef FFMPEGENCODER_H
#define FFMPEGENCODER_H

//#pragma warning (disable: 4996)
#include <QObject>
extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
    #include <libavutil/opt.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}
//#pragma comment(lib,"avcodec.lib")
//#pragma comment(lib,"avdevice.lib")
//#pragma comment(lib,"avfilter.lib")
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avutil.lib")
#include <QQueue>
#include <QMutex>
#include <thread>
struct Frame;
class PreviewPlayer;
class FFmpegEncoder : public QObject
{
    Q_OBJECT
public:
    FFmpegEncoder(QObject *parent,PreviewPlayer *previewPlayer);
    ~FFmpegEncoder();

public:
    int start();
    int pause();
    int stop();
private:
    static void run_thread(void* arg);

public:
    int init(const char *codec_name, int width, int height, int fps, int bitrate, const char* url);

    void encode(Frame* frame);
    void write_trailer(void);
    int get_fps();

    void push_frame(Frame* frame);

private:
    PreviewPlayer *mPreviewPlayer;
    bool mIsRecording = false;//是否在录制中
    bool mIsStop = true;
    std::thread *mThread;

    int mFps;
    AVFormatContext*oFmtCtx = nullptr;
    AVCodecContext *oVideoCodecCtx = nullptr;
    AVStream       *oVideoStream = nullptr;
    SwsContext     *swsCtx_bgra2yuv420p = nullptr;
    AVFrame        *oFrame_yuv420p;
//    int64_t         frameCount = 0;

    QQueue<Frame*> mVideoFrameQ;
    QMutex         mVideoFrameQ_mtx;

signals:

private slots:
};

#endif // FFMPEGENCODER_H
