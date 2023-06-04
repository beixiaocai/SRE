#ifndef ACQSCREEN_H
#define ACQSCREEN_H

#include <QObject>
#include <thread>
class FFmpegEncoder;
class AcqScreen : public QObject
{
    Q_OBJECT
public:
    explicit AcqScreen(QObject *parent,FFmpegEncoder* encoder);
    ~AcqScreen();
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

#endif // ACQSCREEN_H
