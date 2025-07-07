#ifndef ADMININFOFORM_CHAT_H
#define ADMININFOFORM_CHAT_H

#include <QWidget>

namespace Ui {
class AdminInfoForm_Chat;
}

class AdminInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    // 생성자에서 자신이 담당할 회사 이름(roomId)을 받음
    explicit AdminInfoForm_Chat(const QString& companyName, QWidget *parent = nullptr);
    ~AdminInfoForm_Chat();

    void appendMessage(const QString& formattedMessage);
    void onChatTabActivated();

signals:
    // 메시지 전송 시, 어느 회사에 보낼지와 메시지 내용을 함께 전달
    void messageSendRequested(const QString& companyName, const QString& message);

private slots:
    void on_pushButton_admin_clicked();

private:
    void showChatNotification();
    void clearChatNotification();

    Ui::AdminInfoForm_Chat *ui;
    QString m_companyName; // 이 채팅창이 담당하는 회사 이름
};

#endif // ADMININFOFORM_CHAT_H
