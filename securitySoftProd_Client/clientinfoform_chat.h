#ifndef CLIENTINFOFORM_CHAT_H
#define CLIENTINFOFORM_CHAT_H

#include <QWidget>

namespace Ui {
class ClientInfoForm_Chat;
}

class ClientInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm_Chat(QWidget *parent = nullptr);
    ~ClientInfoForm_Chat();

    // MainWindow에서 메시지를 표시하기 위해 호출하는 함수
    void appendMessage(const QString& formattedMessage);
    void onChatTabActivated();

signals:
    // '전송' 버튼 클릭 시 MainWindow에 메시지 전송을 요청하는 시그널
    void messageSendRequested(const QString& message);

private slots:
    // UI의 전송 버튼 클릭 시 내부적으로 호출될 슬롯
    void on_pushButton_client_clicked();

private:
    void showChatNotification();
    void clearChatNotification();

    Ui::ClientInfoForm_Chat *ui;
};

#endif // CLIENTINFOFORM_CHAT_H
