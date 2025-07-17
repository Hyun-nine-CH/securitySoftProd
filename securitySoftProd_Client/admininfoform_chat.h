#ifndef ADMININFOFORM_CHAT_H
#define ADMININFOFORM_CHAT_H

#include <QWidget>
#include <QByteArray>
#include <QBuffer>

namespace Ui {
class AdminInfoForm_Chat;
}

class AdminInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm_Chat(const QString& companyName, QWidget *parent = nullptr);
    ~AdminInfoForm_Chat();

    // 메시지 표시 함수
    void appendMessage(const QBuffer& formattedMessage);

    // 알림 관련 함수들
    void showChatNotification();
    void clearChatNotification();
    void onChatTabActivated();

private slots:
    void on_pushButton_admin_clicked();

private:
    Ui::AdminInfoForm_Chat *ui;
    QString m_companyName; // 이 채팅방이 속한 회사명 (roomId)
};

#endif // ADMININFOFORM_CHAT_H
