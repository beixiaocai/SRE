#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
QT_BEGIN_NAMESPACE;
class QCheckBox;
class QLineEdit;
class QStackedWidget;
QT_END_NAMESPACE;

class Settings : public QDialog
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent);

private:
    void initUi();
    QWidget* initLeftWidget();
    QWidget* initRightWidget();
    QStackedWidget *rightStackedWidget;

    void initCommonWidget();


signals:

};

#endif // SETTINGS_H
