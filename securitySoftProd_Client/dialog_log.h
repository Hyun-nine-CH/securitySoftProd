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
    static Dialog_log* getInstance(QWidget *parent = nullptr);
    ~Dialog_log();

    QJsonObject getUserInfo() const;
    QTcpSocket* getSocket();

    Dialog_log(const Dialog_log&) = delete;
    Dialog_log& operator=(const Dialog_log&) = delete;

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_signUp_clicked();
    void onReadyRead();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    explicit Dialog_log(QWidget *parent = nullptr);

    Ui::Dialog_log *ui;
    static QTcpSocket* socket;
    QByteArray m_buffer;
    QJsonObject m_userInfo;

    static Dialog_log* instance;
};

#endif // DIALOG_LOG_H
