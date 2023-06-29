#ifndef CAPTUREAUDIOTHREAD_H
#define CAPTUREAUDIOTHREAD_H
#include <QThread>

class Recorder;
struct CaptureAudioDevice;
class CaptureAudioThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureAudioThread(QObject *parent,CaptureAudioDevice *audioDevice);
    ~CaptureAudioThread();
protected:
    void run() override;
private:
    Recorder *mRecorder;
    CaptureAudioDevice *mAudioDevice;
     bool mIsStop = true;
signals:

private slots:

};

#endif // CAPTUREAUDIOTHREAD_H
