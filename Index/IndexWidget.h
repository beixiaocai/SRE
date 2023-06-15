#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include <QWidget>
QT_BEGIN_NAMESPACE;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QStackedWidget;
QT_END_NAMESPACE;
class RecordWidget;
class VNCClientWidget;
class VNCServerWidget;

class IndexWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IndexWidget(QWidget *parent);

private:
    void initUi();
    void initSettings();
    QWidget* initLeftWidget();
    //按钮无背景
    const QString left_btn_no_background = ".QPushButton {color:rgb(255,255,255);font-size:15px;border:1px solid rgb(76,76,76); border-radius: 3px;padding: 2px;}\
            .QPushButton:hover {background-color: rgb(53,53,53);}\
            .QPushButton:focus{outline: none;}";
    //按钮无背景-选中
    const QString left_btn_no_background_selected = ".QPushButton {color:rgb(221,58,95);background-color: rgb(53,53,53);font-size:15px;border:1px solid rgb(76,76,76); border-radius: 3px;padding: 2px;}\
            .QPushButton:focus{outline: none;}";
    QPushButton *vncServerBtn;
    QPushButton *vncClientBtn;
    QPushButton *recordBtn;

    QWidget* initRightWidget();
    QStackedWidget *rightStackedWidget;
    RecordWidget *recordWidget;
    VNCClientWidget *vncClientWidget;
    VNCServerWidget *vncServerWidget;

signals:

};

#endif // INDEXWIDGET_H
