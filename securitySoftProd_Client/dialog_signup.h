#ifndef DIALOG_SIGNUP_H
#define DIALOG_SIGNUP_H

#include <QDialog>

// 전방 선언
class QTcpSocket;
namespace Ui {
class Dialog_SignUp;
}

class Dialog_SignUp : public QDialog
{
    Q_OBJECT

public:
    // 생성자에서 부모 위젯과 통신에 사용할 소켓을 받도록 변경
    explicit Dialog_SignUp(QTcpSocket* socket, QWidget *parent = nullptr);
    ~Dialog_SignUp();

private slots:
    // UI 파일의 '가입하기' 버튼 (objectName: pushButton_SignUp)과 연결될 슬롯
    void on_pushButton_SignUp_clicked();

private:
    Ui::Dialog_SignUp *ui;
    QTcpSocket* m_socket; // 로그인 창으로부터 넘겨받은 소켓 포인터
};

#endif // DIALOG_SIGNUP_H
