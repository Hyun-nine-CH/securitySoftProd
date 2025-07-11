#ifndef MAINWINDOW_ADMIN_H
#define MAINWINDOW_ADMIN_H

#include <QMainWindow>
#include <QJsonObject>
#include <QMap>

// 전방 선언
class QTcpSocket;
class AdminInfoForm_Chat;
class AdminInfoForm;
class AdminInfoForm_Prod;
class AdminInfoForm_OL;

namespace Ui { class MainWindow_Admin; }

class MainWindow_Admin : public QMainWindow
{
    Q_OBJECT

public:
    // 생성자에서 소켓과 로그인 성공 시 받은 사용자 정보 전체를 받음
    explicit MainWindow_Admin(QTcpSocket* socket, const QJsonObject& userInfo, QWidget *parent = nullptr);
    ~MainWindow_Admin();

private slots:
    // 서버로부터 오는 모든 메시지를 처리하는 중앙 처리 슬롯
    void handleServerMessage();

    // 각 탭 위젯으로부터 오는 요청을 처리하는 슬롯들
    void sendChatMessage(const QString& companyName, const QString& message);
    void requestProductList();
    void requestMemberList();
    void searchMembers(const QString& company, const QString& department, const QString& phone);
    void requestOrderList();
    void searchOrders(const QString& productName, const QString& dueDate);

    // 툴바 아이콘 클릭 시 탭을 전환하는 슬롯들
    void on_actionClient_Info_triggered();
    void on_actionSecurityProd_Info_triggered();
    void on_actionOrder_Info_triggered();
    void on_actionChatting_Room_triggered();

private:
    // 채팅 탭을 동적으로 생성하거나 기존 탭으로 전환하는 함수
    void createOrSwitchToChatTab(const QString& companyName);
    // 서버에 데이터를 전송하는 공용 헬퍼 함수
    void sendDataToServer(qint64 dataType, const QJsonObject& payload = QJsonObject(), const QString& filename = "");

    Ui::MainWindow_Admin *ui;
    QTcpSocket* m_socket;     // main.cpp로부터 받은 유일한 소켓
    QJsonObject m_userInfo;   // 로그인한 관리자의 정보
    QByteArray m_buffer;

    // 동적으로 생성되는 채팅 탭들을 관리
    QMap<QString, AdminInfoForm_Chat*> m_chatTabs;
    const QString ADMIN_CHAT_ROOM_ID = "@Admins"; // 관리자 채팅방을 위한 고유 ID

    // 각 기능 탭의 포인터를 멤버 변수로 관리
    AdminInfoForm* m_memberTab;
    AdminInfoForm_Prod* m_prodTab;
    AdminInfoForm_OL* m_orderTab;
};

#endif // MAINWINDOW_ADMIN_H
