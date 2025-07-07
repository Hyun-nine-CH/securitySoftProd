#ifndef DIALOG_LOG_H
#define DIALOG_LOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class Dialog_log;
}

class Dialog_log : public QDialog
{
    Q_OBJECT

public:
    // ⭐️ 싱글톤 접근자
    static Dialog_log* getInstance(QWidget *parent = nullptr);
    ~Dialog_log();

    Dialog_log(const Dialog_log&) = delete;
    Dialog_log& operator=(const Dialog_log&) = delete;

private slots:
    void onPushButton_loginClicked();
    void onPushButton_signUpClicked();
    void onReadyRead();

private:
    // ⭐️ 생성자를 private으로
    explicit Dialog_log(QWidget *parent = nullptr);

    Ui::Dialog_log *ui;
    QTcpSocket* socket;

    // ⭐️ 싱글톤 인스턴스
    static Dialog_log* instance;
};

#endif // DIALOG_LOG_H
