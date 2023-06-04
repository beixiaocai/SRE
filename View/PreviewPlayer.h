#ifndef PREVIEWPLAYER_H
#define PREVIEWPLAYER_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
class QImage;
QT_END_NAMESPACE

class PreviewPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewPlayer(QWidget *parent);

private:
    void initUi();
    QVBoxLayout *mainVLayout;
    QLabel *imageLabel;
private:
    QWidget* initNameWidget();
    QWidget* initImageWidget();

signals:
    void setImage(QImage image);
public slots:
    void onSetImage(QImage image);
};

#endif // PREVIEWPLAYER_H
