#ifndef CLIENTINFOFORM_CHAT_H
#define CLIENTINFOFORM_CHAT_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class ClientInfoForm_Chat;
}

class ClientInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    // ⭐️ 생성자에서 companyName은 필요 없으므로 원래대로 복귀
    explicit ClientInfoForm_Chat(QTcpSocket *socket, QWidget *parent = nullptr);
    ~ClientInfoForm_Chat();

    // ⭐️ MainWindow가 호출할 수 있도록 public으로 변경
    void appendMessage(const QString& message);
    void onChatTabActivated();

private slots:
    void sendMessage();
    void handleSocketError(QAbstractSocket::SocketError socketError);

private:
    void showChatNotification();
    void clearChatNotification();

    Ui::ClientInfoForm_Chat *ui;
    QTcpSocket* m_socket;
};

#endif // CLIENTINFOFORM_CHAT_H
