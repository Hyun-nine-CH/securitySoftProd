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
    explicit AdminInfoForm_Chat(const QString& companyName, QWidget *parent = nullptr);
    ~AdminInfoForm_Chat();

    void appendMessage(const QString& formattedMessage);
    void onChatTabActivated();

signals:
    // MainWindow_Admin에게 메시지 전송을 요청하는 시그널
    void messageSendRequested(const QString& companyName, const QString& message);

private slots:
    void on_pushButton_admin_clicked();

private:
    void showChatNotification();
    void clearChatNotification();

    Ui::AdminInfoForm_Chat *ui;
    QString m_companyName; // 이 채팅 탭이 담당하는 회사 이름
};

#endif // ADMININFOFORM_CHAT_H
