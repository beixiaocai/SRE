#ifndef CLIENTTABWIDGET_H
#define CLIENTTABWIDGET_H
#include <QTabWidget>

class CilentTabWidget : public  QTabWidget
{
    Q_OBJECT
public:
    explicit CilentTabWidget(QWidget *parent);

public:
    void addDesktopTab();

signals:
public slots:
    void onCloseTab(int index);

};

#endif // CLIENTTABWIDGET_H
