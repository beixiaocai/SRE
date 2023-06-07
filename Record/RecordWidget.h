#ifndef RECORDWIDGET_H
#define RECORDWIDGET_H

#include <QWidget>
#include <QImage>
#include <QDateTime>

QT_BEGIN_NAMESPACE
class QTimer;
class QLabel;
class QPushButton;
QT_END_NAMESPACE
class Assistant;

class RecordWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RecordWidget(QWidget *parent);
    ~RecordWidget();

private:
    void initUi();
    QLabel *imageLabel;//渲染录屏画面

    QPushButton *startBtn;
    QPushButton *stopBtn;
    QPushButton *pauseBtn;

    QLabel    *durationLabel;
    QTimer    *durationTimer;

    QWidget* initRecordSourceWidget();

private:
    Assistant  *mAssist;
    bool      isRecording;//是否在录制中
    QDateTime recordStartDate;//开始录制的时间

signals:

public slots:
    void onSetImage(QImage image);

};

#endif // RECORDWIDGET_H
