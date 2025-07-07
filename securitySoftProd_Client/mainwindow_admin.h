#ifndef MAINWINDOW_ADMIN_H
#define MAINWINDOW_ADMIN_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMap>

class AdminInfoForm_Chat;
namespace Ui { class MainWindow_Admin; }

class MainWindow_Admin : public QMainWindow
{
    Q_OBJECT

public:
    // ⭐️ 생성자에서 소켓과 관리자 ID를 받음
    explicit MainWindow_Admin(QTcpSocket* socket, qint64 adminId, QWidget *parent = nullptr);
    ~MainWindow_Admin();

private slots:
    void handleServerMessage();
    // ⭐️ 채팅 탭의 요청을 받아 메시지를 전송하는 슬롯
    void sendChatMessage(const QString& companyName, const QString& message);

private:
    void createOrSwitchToChatTab(const QString& companyName);
    // ⭐️ 데이터를 바이너리 형식으로 전송하는 헬퍼 함수
    void writeData(qint64 dataType, const QString& roomId, qint64 clientId, const QString& message);

    Ui::MainWindow_Admin *ui;
    QTcpSocket* m_socket;
    qint64 m_myUserId; // 관리자 자신의 ID (e.g., 1001)
    QMap<QString, AdminInfoForm_Chat*> m_chatTabs;
    QByteArray m_buffer; // ⭐️ 소켓 데이터 수신 버퍼
};

#endif // MAINWINDOW_ADMIN_H
