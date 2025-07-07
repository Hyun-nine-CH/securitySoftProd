#ifndef ADMININFOFORM_CHAT_H
#define ADMININFOFORM_CHAT_H

#include <QWidget>

// ⭐️ QTcpSocket 헤더는 더 이상 필요 없음
// #include <QTcpSocket>

namespace Ui {
class AdminInfoForm_Chat;
}

class AdminInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    // ⭐️ 생성자에서 소켓 포인터 대신 회사 이름만 받음
    explicit AdminInfoForm_Chat(const QString& companyName, QWidget *parent = nullptr);
    ~AdminInfoForm_Chat();

    // MainWindow가 호출하는 함수들
    void appendMessage(const QString& formattedMessage);
    void onChatTabActivated();

signals:
    // ⭐️ MainWindow_Admin에게 메시지 전송을 요청하는 시그널
    void messageSendRequested(const QString& companyName, const QString& message);

private slots:
    // ⭐️ 내부적으로만 사용되는 슬롯
    void on_sendButton_clicked();

private:
    void showChatNotification();
    void clearChatNotification();

    Ui::AdminInfoForm_Chat *ui;
    QString m_companyName; // 이 채팅창이 담당하는 회사 이름
};

#endif // ADMININFOFORM_CHAT_H
