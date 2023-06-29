#ifndef CAPTUREVIDEOTHREAD_H
#define CAPTUREVIDEOTHREAD_H
#include <QThread>

class Recorder;
struct CaptureVideoDevice;
class CaptureVideoThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureVideoThread(QObject *parent,CaptureVideoDevice *videoDevice);
    ~CaptureVideoThread();
protected:
    void run() override;
private:
    Recorder *mRecorder;
    CaptureVideoDevice *mVideoDevice;
    bool mIsStop = true;
signals:

private slots:

};

#endif // CAPTUREVIDEOTHREAD_H
