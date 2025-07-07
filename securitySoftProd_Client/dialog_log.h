#ifndef DIALOG_LOG_H
#define DIALOG_LOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QByteArray> // QByteArray 헤더 추가

namespace Ui {
class Dialog_log;
}

class Dialog_log : public QDialog
{
    Q_OBJECT

public:
    static Dialog_log* getInstance(QWidget *parent = nullptr);
    ~Dialog_log();

    Dialog_log(const Dialog_log&) = delete;
    Dialog_log& operator=(const Dialog_log&) = delete;

private slots:
    void on_pushButton_login_clicked(); // UI 파일과 자동 연결되도록 이름 변경
    void on_pushButton_signUp_clicked();
    void onReadyRead();

private:
    explicit Dialog_log(QWidget *parent = nullptr);

    Ui::Dialog_log *ui;
    QTcpSocket* socket;
    QByteArray  m_buffer; // 서버로부터 오는 데이터를 임시 저장할 버퍼
    static Dialog_log* instance;
};

#endif // DIALOG_LOG_H
