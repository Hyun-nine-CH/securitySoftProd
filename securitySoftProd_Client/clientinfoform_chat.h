#ifndef CLIENTINFOFORM_CHAT_H
#define CLIENTINFOFORM_CHAT_H

#include <QWidget>
#include <QJsonObject>
#include <QByteArray>
#include <QKeyEvent>
#include <QTcpSocket>
#include <QBuffer>

namespace Ui {
class ClientInfoForm_Chat;
}

class ClientInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm_Chat(QWidget *parent = nullptr);
    ~ClientInfoForm_Chat();

    // 메시지 표시 함수
    void appendMessage(const QBuffer &formattedMessage);

    // 알림 관련 함수들
    void showChatNotification();
    // void clearChatNotification();
    // void onChatTabActivated();
private slots:
    void on_pushButton_client_clicked();
    void on_pushButton_fileClient_clicked();
    // void appendMessage(const QBuffer &buffer);
    //void on_pushButton_admin_clicked();

private:
    Ui::ClientInfoForm_Chat *ui;
    // 엔터키 이벤트 처리를 위한 이벤트 필터
    bool eventFilter(QObject *watched, QEvent *event) override;

};

#endif // CLIENTINFOFORM_CHAT_H
