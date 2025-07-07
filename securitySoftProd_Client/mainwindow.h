#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QByteArray>

class ClientInfoForm_Chat;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 생성자에서 로그인 정보를 받도록 변경
    explicit MainWindow(QTcpSocket* socket, const QString& roomId, qint64 clientId, const QString& managerName, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 서버에서 오는 모든 메시지를 처리하는 단일 슬롯
    void handleServerMessage();
    // 채팅 탭의 요청을 받아 메시지를 전송하는 슬롯
    void sendChatMessage(const QString& message);

private:
    Ui::MainWindow *ui;
    QTcpSocket* m_socket;
    QByteArray  m_buffer; // 소켓 데이터 수신 버퍼

    // 로그인 시 받은 사용자 정보
    QString m_roomId;      // 자신의 소속 회사 (e.g., "A Com")
    qint64  m_clientId;    // 자신의 ID (e.g., 1)
    QString m_managerName; // 자신의 이름 (e.g., "김철수")

    ClientInfoForm_Chat* m_chatTab;
};

#endif // MAINWINDOW_H
