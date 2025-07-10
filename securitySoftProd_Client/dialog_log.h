#ifndef DIALOG_LOG_H
#define DIALOG_LOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QJsonObject> // QJsonObject 사용을 위해 헤더 추가

namespace Ui {
class Dialog_log;
}

class Dialog_log : public QDialog
{
    Q_OBJECT

public:
    // 싱글톤 패턴의 유일한 접근자
    static Dialog_log* getInstance(QWidget *parent = nullptr);
    ~Dialog_log();

    // 외부(main.cpp)에서 로그인 정보에 접근하기 위한 함수들
    QJsonObject getUserInfo() const; // 저장된 사용자 정보를 반환
    QTcpSocket* getSocket();         // 메인 윈도우에 소켓을 넘겨주기 위한 함수

    // 복사 및 대입을 막아 싱글톤의 유일성을 보장
    Dialog_log(const Dialog_log&) = delete;
    Dialog_log& operator=(const Dialog_log&) = delete;

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_signUp_clicked();
    void onReadyRead();

protected:
    // Enter 키 이벤트를 감지하기 위한 이벤트 필터
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    // 생성자를 private으로 선언하여 외부 생성을 차단
    explicit Dialog_log(QWidget *parent = nullptr);

    Ui::Dialog_log *ui;
    QTcpSocket* socket;      // 프로그램 전체에서 사용할 유일한 소켓
    QByteArray m_buffer;

    // 로그인 성공 시 서버로부터 받은 사용자 정보를 저장할 멤버 변수
    QJsonObject m_userInfo;

    // 유일한 인스턴스를 저장할 static 포인터
    static Dialog_log* instance;
};

#endif // DIALOG_LOG_H
