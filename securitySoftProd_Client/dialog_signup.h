#ifndef DIALOG_SIGNUP_H
#define DIALOG_SIGNUP_H

#include <QDialog>
#include <QTcpSocket>
#include <QJsonObject>

namespace Ui {
class Dialog_SignUp;
}

class Dialog_SignUp : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_SignUp(QTcpSocket* socket, QWidget *parent = nullptr);
    ~Dialog_SignUp();

private slots:
    void on_pushButton_SignUp_clicked();
    void on_pushButton_IDcheck_clicked();
    void on_pushButton_DoubleCheck_clicked();
    void onReadyRead(); // 서버 응답 처리

private:
    Ui::Dialog_SignUp *ui;
    QTcpSocket* m_socket; // 로그인 창으로부터 넘겨받은 소켓 포인터
    QByteArray m_buffer; // 서버로부터 받은 데이터를 저장할 버퍼
    bool m_idChecked; // ID 중복 확인 완료 여부
    bool m_pwChecked; // 비밀번호 확인 완료 여부
};

#endif // DIALOG_SIGNUP_H
