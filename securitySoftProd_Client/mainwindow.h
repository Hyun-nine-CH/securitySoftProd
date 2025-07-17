#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
// 전방 선언
class QTcpSocket;
class ClientInfoForm_Chat;
class ClientInfoForm_Prod;
class ClientInfoForm; // 주문하기 탭

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 생성자에서 소켓과 로그인 성공 시 받은 사용자 정보 전체를 받음
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 각 탭 위젯으로부터 오는 요청을 처리하는 슬롯들 (아직 분할되지 않은 탭들을 위한)
    void searchProducts(const QString& name, const QString& price, const QString& dueDate);
    void submitOrder(const QJsonObject& orderData);

    // 툴바 아이콘 클릭 시 탭을 전환하는 슬롯들
    void on_actionOrder_Info_triggered();
    void on_actionChatting_Room_triggered();
    void on_actionQuit_triggered(); // 종료 버튼 슬롯 추가

    //탭 바뀔때마다 데이터 로드
    void onTabChanged(int index);

    void searchOrders(const QString& productName, const QString& price, const QString& dueDate);

private:
    Ui::MainWindow *ui;
    QByteArray m_buffer;      // 서버로부터 받은 데이터를 저장할 버퍼

    // 각 탭 위젯의 포인터를 멤버 변수로 관리
    ClientInfoForm* m_orderTab;
    ClientInfoForm_Chat* m_chatTab;
};

#endif // MAINWINDOW_H
