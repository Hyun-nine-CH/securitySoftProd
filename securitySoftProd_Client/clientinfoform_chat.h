#ifndef CLIENTINFOFORM_CHAT_H
#define CLIENTINFOFORM_CHAT_H

#include <QWidget>
#include <QJsonObject>
#include <QByteArray>
#include <QKeyEvent>
#include <QTcpSocket>

namespace Ui {
class ClientInfoForm_Chat;
}

class ClientInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm_Chat(QTcpSocket* socket, const QJsonObject& userInfo, QWidget *parent = nullptr);
    ~ClientInfoForm_Chat();

    // 메시지 표시 함수
    void appendMessage(const QString& formattedMessage);

    // 알림 관련 함수들
    void showChatNotification();
    void clearChatNotification();
    void onChatTabActivated();

    // 서버에서 받은 데이터 처리
    void handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename);

private slots:
    void on_pushButton_client_clicked();
    void on_pushButton_fileClient_clicked();

private:
    Ui::ClientInfoForm_Chat *ui;
    QTcpSocket* m_socket;      // MainWindow로부터 받은 소켓 포인터
    QJsonObject m_userInfo;    // 사용자 정보
    QByteArray m_buffer;       // 데이터 수신 버퍼

    // 채팅 기록 요청
    void requestChatHistory();

    // 서버에 데이터 전송 헬퍼 함수
    void sendDataToServer(qint64 dataType, const QJsonObject& payload, const QString& filename);

    // 엔터키 이벤트 처리를 위한 이벤트 필터
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // CLIENTINFOFORM_CHAT_H
