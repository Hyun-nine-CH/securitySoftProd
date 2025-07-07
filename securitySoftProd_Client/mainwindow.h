#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

class ClientInfoForm_Chat;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // ⭐️ 생성자에서 소켓, 회사이름, 클라이언트ID를 받음
    explicit MainWindow(QTcpSocket* socket, const QString& companyName, qint64 clientId, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleServerMessage();
    // ⭐️ 채팅 탭의 요청을 받아 메시지를 전송하는 슬롯
    void sendChatMessage(const QString& message);

private:
    // ⭐️ 데이터를 바이너리 형식으로 전송하는 헬퍼 함수
    void writeData(qint64 dataType, const QString& roomId, qint64 clientId, const QString& message);

    Ui::MainWindow *ui;
    QTcpSocket* m_socket;
    QString m_companyName; // 자신의 소속 회사 이름 (RoomId)
    qint64 m_myUserId;     // 자신의 ID (ClientId)
    ClientInfoForm_Chat* m_chatTab;
    QByteArray m_buffer;   // ⭐️ 소켓 데이터 수신 버퍼
};

#endif // MAINWINDOW_H
