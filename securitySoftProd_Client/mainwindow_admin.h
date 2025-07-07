#ifndef MAINWINDOW_ADMIN_H
#define MAINWINDOW_ADMIN_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMap>

// 전방 선언
class AdminInfoForm_Chat;
namespace Ui { class MainWindow_Admin; }

class MainWindow_Admin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_Admin(QTcpSocket* socket, QWidget *parent = nullptr);
    ~MainWindow_Admin();

private slots:
    // ⭐️ 서버에서 오는 모든 메시지를 처리하는 단일 슬롯
    void handleServerMessage();

private:
    // ⭐️ 특정 회사의 채팅 탭을 만들거나 찾는 함수
    void createOrSwitchToChatTab(const QString& companyName);

    Ui::MainWindow_Admin *ui;
    QTcpSocket* m_socket;
    // ⭐️ Key: 회사이름, Value: 해당 회사 채팅창 위젯
    QMap<QString, AdminInfoForm_Chat*> m_chatTabs;
};

#endif // MAINWINDOW_ADMIN_H
