#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent);
    ~ClientDialog();
private:
    void initUi();

protected:
    void closeEvent(QCloseEvent *) override;

signals:

public slots:

};

#endif // CLIENTDIALOG_H
