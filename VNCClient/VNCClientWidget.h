#ifndef VNCCLIENTWIDGET_H
#define VNCCLIENTWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE;
class QListWidget;
QT_END_NAMESPACE;
class ClientDialog;

class VNCClientWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VNCClientWidget(QWidget *parent);
private:
    void initUi();
    QWidget* initLeftWidget();
    QListWidget *deviceListWidget;

    QWidget* initRightWidget();

    ClientDialog *clientDialog;

signals:

};

#endif // VNCCLIENTWIDGET_H
