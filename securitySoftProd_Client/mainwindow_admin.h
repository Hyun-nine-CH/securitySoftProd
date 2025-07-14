// #ifndef MAINWINDOW_ADMIN_H
// #define MAINWINDOW_ADMIN_H

// #include <QMainWindow>
// #include <QJsonObject>
// #include <QMap>

// // 전방 선언
// class QTcpSocket;
// class AdminInfoForm_Prod;
// class AdminInfoForm_OL;
// class AdminInfoForm_Chat;
// class AdminInfoForm;

// namespace Ui { class MainWindow_Admin; }

// class MainWindow_Admin : public QMainWindow
// {
//     Q_OBJECT

// public:
//     // 생성자에서 소켓과 로그인 성공 시 받은 사용자 정보 전체를 받음
//     explicit MainWindow_Admin(QWidget *parent = nullptr);
//     ~MainWindow_Admin();

// private slots:
//     // 서버로부터 오는 모든 메시지를 처리하는 중앙 처리 슬롯
//     void handleServerMessage();

//     // 각 탭 위젯으로부터 오는 요청을 처리하는 슬롯들
//     void requestProductList();
//     void requestClientList();
//     void requestOrderList();
//     void sendChatMessage(const QString& roomId, const QString& message);

//     // 툴바 아이콘 클릭 시 탭을 전환하는 슬롯들
//     void on_actionSecurityProd_Info_triggered();
//     void on_actionOrder_Info_triggered();
//     void on_actionChatting_Room_triggered();
//     void on_actionClient_Info_triggered();
//     void on_actionQuit_triggered();

// private:
//     // 서버에 데이터를 전송하는 공용 헬퍼 함수
//     void sendDataToServer(qint64 dataType, const QJsonObject& payload = QJsonObject(), const QString& filename = "");

//     // 새로운 채팅방 탭 생성 함수
//     void createNewChatTab(const QString& roomId, const QString& initialMessage = "");

//     Ui::MainWindow_Admin *ui;
//     QTcpSocket* m_socket;     // main.cpp로부터 받은 유일한 소켓
//     QJsonObject m_userInfo;   // 로그인한 사용자의 정보
//     QByteArray m_buffer;      // 서버로부터 받은 데이터를 저장할 버퍼

//     // 각 탭 위젯의 포인터를 멤버 변수로 관리
//     AdminInfoForm_Prod* m_prodTab;
//     AdminInfoForm_OL* m_orderTab;
//     AdminInfoForm_Chat* m_adminChatTab;  // 관리자 전용 채팅방
//     AdminInfoForm* m_clientTab;

//     // 채팅방 탭 관리 (roomId를 키로 사용)
//     QMap<QString, AdminInfoForm_Chat*> m_chatTabs;
// };

// #endif // MAINWINDOW_ADMIN_H
