#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QTimer;
class QLabel;
class QPushButton;
QT_END_NAMESPACE

class Recorder;
class PreviewPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void initMenu();
    void initUi();
    QVBoxLayout *mainVLayout;

    QPushButton *startBtn;
    QPushButton *stopBtn;
    QPushButton *pauseBtn;

    QLabel    *durationLabel;
    QTimer    *durationTimer;

    //源头菜单项
    QWidget* initSourceWidget();



private:
    PreviewPlayer *mPreviewPlayer;
    Recorder *mRecorder;

    bool      isRecording;//是否在录制中
    QDateTime recordStartDate;//开始录制的时间

};
#endif // MAINWINDOW_H
