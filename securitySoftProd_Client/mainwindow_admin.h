#ifndef MAINWINDOW_ADMIN_H
#define MAINWINDOW_ADMIN_H

#include <QMainWindow>
#include <QJsonObject>
#include <QMap>
#include <QBuffer>
#include <QListWidget>
// 전방 선언
class QTcpSocket;
class AdminInfoForm_Prod;
class AdminInfoForm_OL;
class AdminInfoForm_Chat;
class AdminInfoForm;

namespace Ui { class MainWindow_Admin; }

class MainWindow_Admin : public QMainWindow
{
    Q_OBJECT

public:
    // 생성자에서 소켓과 로그인 성공 시 받은 사용자 정보 전체를 받음
    explicit MainWindow_Admin(QWidget *parent = nullptr);
    ~MainWindow_Admin();

private slots:
    // 각 탭 위젯으로부터 오는 요청을 처리하는 슬롯들
    //void requestClientList();
    //void requestOrderList();
    //void sendChatMessage(const QString& roomId, const QString& message);

    // 툴바 아이콘 클릭 시 탭을 전환하는 슬롯들
    void on_actionSecurityProd_Info_triggered();
    void on_actionOrder_Info_triggered();
    void on_actionChatting_Room_triggered();
    void on_actionClient_Info_triggered();
    void on_actionQuit_triggered();

    void CreateChatRoom(const QBuffer &buffer);
    void RequestActiveUserList(int index);
    void ReceiveActiveUserList(const QBuffer &buffer);

private:
    Ui::MainWindow_Admin *ui;
    // 각 탭 위젯의 포인터를 멤버 변수로 관리
    AdminInfoForm_Prod* m_prodTab;
    AdminInfoForm_OL* m_orderTab;
    AdminInfoForm* m_clientTab;

    // 채팅방 탭 관리 (roomId를 키로 사용)
    void onTabChanged(int index);

signals:
    void InviteUser(QString UserId);

};

#endif // MAINWINDOW_ADMIN_H
