#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "clientinfoform_prod.h"
#include "clientinfoform.h" // ClientInfoForm_Order 대신 일반 ClientInfoForm 사용
#include "clientinfoform_chat.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QMessageBox>
#include <QTimer> // QTimer::singleShot 사용을 위해 추가
#include "Protocol.h"
#include "communication.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 윈도우 제목 설정 - 회사명과 사용자 이름 표시
    QString title = QString("클라이언트 - %1 (%2)")
                        .arg(Communication::getInstance()->getUserInfo()["RoomId"].toString())
                        .arg(Communication::getInstance()->getUserInfo()["manager"].toString());
    this->setWindowTitle(title);

    // 툴바 액션(아이콘 버튼)과 탭 전환 슬롯 연결
    connect(ui->actionOrder_Info, &QAction::triggered, this, &MainWindow::on_actionOrder_Info_triggered);
    connect(ui->actionChatting_Room, &QAction::triggered, this, &MainWindow::on_actionChatting_Room_triggered);
    //connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::on_actionQuit_triggered);

    // 1. 제품 정보 탭 생성 및 시그널 연결 (Prod 탭은 아직 분할되지 않았다고 가정)
    //m_prodTab = new ClientInfoForm_Prod(this);
    // connect(m_prodTab, &ClientInfoForm_Prod::productListRequested, this, &MainWindow::requestProductList);
    // connect(m_prodTab, &ClientInfoForm_Prod::searchProductsRequested, this, &MainWindow::searchProducts);
    // ui->tabWidget->addTab(m_prodTab, tr("제품 정보"));

    // 2. 주문 하기 탭 생성 및 시그널 연결
    m_orderTab = new ClientInfoForm(this);
    //connect(m_orderTab, &ClientInfoForm::orderSubmitted, this, &MainWindow::submitOrder);
    //connect(m_orderTab, &ClientInfoForm::orderListRequested, this, &MainWindow::requestOrderList);
    //connect(m_orderTab, &ClientInfoForm::searchOrdersRequested, this, &MainWindow::searchOrders);
    ui->tabWidget->addTab(m_orderTab, tr("주문 하기"));

    // 3. 채팅방 탭 생성 (책임 분할 적용)
    // 채팅 탭이 직접 소켓과 유저 정보를 받아 통신을 처리합니다.
    m_chatTab = new ClientInfoForm_Chat();
    // m_chatTab으로부터 messageSendRequested 시그널을 받을 필요 없음 (이제 chatTab이 직접 보냄)
    ui->tabWidget->addTab(m_chatTab, tr("채팅방"));
    // QTabWidget의 시그널 연결
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    ui->tabWidget->setCurrentIndex(0); // 기본으로 첫 번째 탭을 보여줌

    onTabChanged(ui->tabWidget->currentIndex()); //강제 신호 보내기(초기화 데이터 로드하기 위해서)

    // 탭 전환 시 채팅방 알림 아이콘을 지우는 로직 (MainWindow가 직접 탭 활성화를 감지)
    // connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
    //     if (ui->tabWidget->widget(index) == m_chatTab) {
    //         m_chatTab->onChatTabActivated(); // 채팅 탭 활성화 시 알림 제거 요청
    //     }
    // });

    // 초기 제품 목록 요청 (500ms 후)
    //QTimer::singleShot(500, this, &MainWindow::requestProductList);
}

MainWindow::~MainWindow() {
    // m_socket은 Dialog_log 싱글톤이 관리하므로 여기서 delete하지 않음
    delete ui;
}

// --- 툴바 액션 슬롯 구현 ---
void MainWindow::on_actionOrder_Info_triggered() { ui->tabWidget->setCurrentWidget(m_orderTab); }
void MainWindow::on_actionChatting_Room_triggered() { ui->tabWidget->setCurrentWidget(m_chatTab); }

void MainWindow::on_actionQuit_triggered() {
    // 서버에 로그아웃 메시지 전송
    //QJsonObject logoutData;
    //logoutData["action"] = "logout";
    //logoutData["userId"] = Communication::getInstance()->getUserInfo()["id"].toString();

    // 로그아웃 요청 전송 (Logout_Request 프로토콜 사용)
    //sendDataToServer(Protocol::Logout_Request, logoutData, "logout");

    // 소켓 연결 종료
    if (Communication::getInstance()->getSocket() && Communication::getInstance()->getSocket()->state() == QAbstractSocket::ConnectedState) {
        Communication::getInstance()->getSocket()->disconnectFromHost();
        // 연결 끊어질 때까지 최대 1초 대기 (선택 사항)
        if (Communication::getInstance()->getSocket()->state() != QAbstractSocket::UnconnectedState)
            Communication::getInstance()->getSocket()->waitForDisconnected(1000);
    }

    // 창 닫기
    this->close();
}

void MainWindow::onTabChanged(int index)
{
    if (index == 0) {
        m_orderTab->getToolBox()->setCurrentIndex(0);
        m_orderTab->ProductTableSet();
        m_orderTab->handleIncomingData();// 데이터로드
    } else if (index == 1) {
        // 두 번째 탭이 선택되었고 아직 데이터가 로드되지 않았다면
        // Communication::getInstance().requestTab2Data();
        // ui->tab2Widget->loadData();
    }
}

void MainWindow::searchProducts(const QString& name, const QString& price, const QString& dueDate) {
    QJsonObject searchData;
    searchData["name"] = name;
    searchData["price"] = price;
    searchData["dueDate"] = dueDate;
}

void MainWindow::submitOrder(const QJsonObject& orderData) {
    QMessageBox::information(this, "주문 완료", "주문이 성공적으로 접수되었습니다.");

    // 주문 후 목록 갱신을 위해 주문 목록 다시 요청
}

// 주문 검색 함수 추가
void MainWindow::searchOrders(const QString& productName, const QString& price, const QString& dueDate) {
    QJsonObject searchData;
    searchData["productName"] = productName;
    searchData["price"] = price;
    searchData["dueDate"] = dueDate;
}

