#ifndef CAPTUREVIDEO_H
#define CAPTUREVIDEO_H

#include <QObject>
#include <thread>
class FFmpegEncoder;
class Assistant;

class CaptureVideo : public QObject
{
    Q_OBJECT
public:
    explicit CaptureVideo(QObject *parent,FFmpegEncoder* encoder);
    ~CaptureVideo();
private:
    static void run_thread(void* arg);
public:
    int start();
    int pause();
    int stop();
private:
    Assistant* mAssist;
    FFmpegEncoder* mEncoder;
    bool mIsRecording = false;//是否在录制中
    bool mIsStop = true;
    std::thread *mThread;

signals:

private slots:

};

#endif // CAPTUREVIDEO_H
