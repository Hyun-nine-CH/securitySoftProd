#ifndef ADMININFOFORM_CHAT_H
#define ADMININFOFORM_CHAT_H

#include <QWidget>
#include <QByteArray>

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
    void appendMessage(const QString& formattedMessage);

    // 알림 관련 함수들
    void showChatNotification();
    void clearChatNotification();
    void onChatTabActivated();

    // 서버에서 받은 데이터 처리
    void handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename);

signals:
    // 메시지 전송 요청 시그널 (회사명과 메시지 내용 전달)
    void messageSendRequested(const QString& roomId, const QString& message);
    void clientListRequested();

private slots:
    //void on_pushButton_admin_clicked();
    void displayRoomList();

private:
    Ui::AdminInfoForm_Chat *ui;
    QString m_companyName; // 이 채팅방이 속한 회사명 (roomId)
};

#endif // ADMININFOFORM_CHAT_H
