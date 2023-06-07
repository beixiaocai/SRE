#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
QT_BEGIN_NAMESPACE;
class QVBoxLayout;
QT_END_NAMESPACE;

class About : public QDialog
{
    Q_OBJECT
public:
    explicit About(QWidget *parent);
private:
    QVBoxLayout *mainVLayout;

    void initTopUi();
    void initBottomUi();

signals:

};

#endif // ABOUT_H
