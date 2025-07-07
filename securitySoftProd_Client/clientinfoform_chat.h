#ifndef CLIENTINFOFORM_CHAT_H
#define CLIENTINFOFORM_CHAT_H

#include <QWidget>

// ⭐️ QTcpSocket 헤더는 더 이상 필요 없음
// #include <QTcpSocket>

namespace Ui {
class ClientInfoForm_Chat;
}

class ClientInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm_Chat(QWidget *parent = nullptr);
    ~ClientInfoForm_Chat();

    void appendMessage(const QString& formattedMessage);
    void onChatTabActivated();

signals:
    // ⭐️ MainWindow에게 메시지 전송을 요청하는 시그널
    void messageSendRequested(const QString& message);

private slots:
    void on_sendButton_clicked();

private:
    void showChatNotification();
    void clearChatNotification();

    Ui::ClientInfoForm_Chat *ui;
};

#endif // CLIENTINFOFORM_CHAT_H
