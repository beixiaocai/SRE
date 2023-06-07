#ifndef CAPTUREAUDIO_H
#define CAPTUREAUDIO_H

#include <QObject>
#include <thread>
class FFmpegEncoder;

class CaptureAudio : public QObject
{
    Q_OBJECT
public:
    explicit CaptureAudio(QObject *parent,FFmpegEncoder* encoder);
    ~CaptureAudio();
private:
    static void run_thread(void* arg);
public:
    int start();
    int pause();
    int stop();
private:
    FFmpegEncoder* mEncoder;
    bool mIsRecording = false;//是否在录制中
    bool mIsStop = true;
    std::thread *mThread;

signals:

private slots:

};

#endif // CAPTUREAUDIO_H
