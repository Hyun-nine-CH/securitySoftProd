#ifndef MAINWINDOW_ADMIN_H
#define MAINWINDOW_ADMIN_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMap>
#include <QByteArray>

class AdminInfoForm_Chat;
namespace Ui { class MainWindow_Admin; }

class MainWindow_Admin : public QMainWindow
{
    Q_OBJECT

public:
    // 생성자에서 관리자 정보를 받도록 변경
    explicit MainWindow_Admin(QTcpSocket* socket, qint64 clientId, const QString& managerName, QWidget *parent = nullptr);
    ~MainWindow_Admin();

private slots:
    void handleServerMessage();
    // 특정 회사 또는 관리자 채팅방에 메시지를 보내는 슬롯
    void sendChatMessage(const QString& companyName, const QString& message);

private:
    // 특정 회사의 채팅 탭을 만들거나 찾는 함수
    void createOrSwitchToChatTab(const QString& companyName);

    Ui::MainWindow_Admin *ui;
    QTcpSocket* m_socket;
    QByteArray m_buffer;

    // 관리자 자신의 정보
    qint64  m_clientId;
    QString m_managerName;

    // Key: 회사이름(RoomId), Value: 해당 회사 채팅창 위젯
    QMap<QString, AdminInfoForm_Chat*> m_chatTabs;

    // 관리자 채팅방을 위한 고유 ID
    const QString ADMIN_CHAT_ROOM_ID = "@Admins";
};

#endif // MAINWINDOW_ADMIN_H
