#ifndef CAPTUREAUDIO_H
#define CAPTUREAUDIO_H

#include <QObject>
#include <thread>
class CaptureFFmpegEncoder;
class CaptureAssistant;

class CaptureAudio : public QObject
{
    Q_OBJECT
public:
    explicit CaptureAudio(QObject *parent,CaptureFFmpegEncoder* encoder);
    ~CaptureAudio();
private:
    static void run_thread(void* arg);
public:
    int start();
    int pause();
    int stop();
private:
    CaptureAssistant* mAssistant;
    CaptureFFmpegEncoder* mEncoder;
    bool mIsRecord = false;//是否在录制中
    bool mIsStop = true;
    std::thread *mThread;

signals:

private slots:

};

#endif // CAPTUREAUDIO_H
