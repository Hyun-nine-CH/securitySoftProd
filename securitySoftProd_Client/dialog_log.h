#ifndef DIALOG_LOG_H
#define DIALOG_LOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QJsonObject>

namespace Ui {
class Dialog_log;
}

class Dialog_log : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_log(QWidget *parent = nullptr);
    ~Dialog_log();

    int IsLogin();

private slots:
    void on_pushButton_login_clicked();
    void LoginPass();
    void LoginFail();

private:
    Ui::Dialog_log *ui;
};

#endif // DIALOG_LOG_H
