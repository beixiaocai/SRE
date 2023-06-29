#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private:
    virtual void resizeEvent(QResizeEvent* event) override;
};
#endif // MAINWINDOW_H
