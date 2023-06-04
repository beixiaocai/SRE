#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
QT_BEGIN_NAMESPACE;
class QVBoxLayout;
class QCheckBox;
class QLineEdit;
QT_END_NAMESPACE;
class ComSpinWidget;

class Settings : public QDialog
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent);

private:
    QVBoxLayout *mainVLayout;

    void initBottomUi();

signals:

};

#endif // SETTINGS_H
