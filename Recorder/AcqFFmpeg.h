#ifndef RECORDAUDIO_H
#define RECORDAUDIO_H

#include <QObject>
#include <thread>
class FFmpegEncoder;

class AcqFFmpeg : public QObject
{
    Q_OBJECT
public:
    explicit AcqFFmpeg(QObject *parent,FFmpegEncoder* encoder);
    ~AcqFFmpeg();
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

#endif // RECORDAUDIO_H
