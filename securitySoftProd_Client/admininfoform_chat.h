#ifndef ADMININFOFORM_CHAT_H
#define ADMININFOFORM_CHAT_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class AdminInfoForm_Chat;
}

class AdminInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    // ⭐️ 생성자 변경: 소켓과 함께 '회사 이름'을 받음
    explicit AdminInfoForm_Chat(QTcpSocket *socket, const QString& companyName, QWidget *parent = nullptr);
    ~AdminInfoForm_Chat();

    // ⭐️ MainWindow가 호출할 수 있도록 public으로 변경
    void appendMessage(const QString& message);
    void onChatTabActivated();

private slots:
    void sendMessage();
    void handleSocketError(QAbstractSocket::SocketError socketError);

private:
    void showChatNotification();
    void clearChatNotification();

    Ui::AdminInfoForm_Chat *ui;
    QTcpSocket* m_socket;
    QString m_companyName; // ⭐️ 이 채팅창이 담당하는 회사 이름
};

#endif // ADMININFOFORM_CHAT_H
