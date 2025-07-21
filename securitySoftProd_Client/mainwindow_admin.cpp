#include "mainwindow_admin.h"
#include "ui_mainwindow_admin.h"
#include "admininfoform_prod.h"
#include "admininfoform_ol.h"
#include "admininfoform_chat.h"
#include "admininfoform.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <QMap>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include "communication.h"

MainWindow_Admin::MainWindow_Admin(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow_Admin)
{
    ui->setupUi(this);

    // 윈도우 제목 설정 - 관리자 정보 표시
    QString title = QString("관리자 - %1 (%2)")
                        .arg(Communication::getInstance()->getUserInfo()["department"].toString())
                        .arg(Communication::getInstance()->getUserInfo()["manager"].toString());
    this->setWindowTitle(title);

    // 툴바 액션(아이콘 버튼)과 탭 전환 슬롯 연결
    connect(ui->actionSecurityProd_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionSecurityProd_Info_triggered);
    connect(ui->actionOrder_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionOrder_Info_triggered);
    connect(ui->actionChatting_Room, &QAction::triggered, this, &MainWindow_Admin::on_actionChatting_Room_triggered);
    connect(ui->actionClient_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionClient_Info_triggered);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow_Admin::on_actionQuit_triggered);
    connect(Communication::getInstance(),&Communication::ReceviceChatRoomInfo, this, &MainWindow_Admin::CreateChatRoom);
    connect(this, &MainWindow_Admin::InviteUser, &Communication::RequestInviteUser); //채팅초대
    // 1. 제품 정보 탭 생성 및 시그널 연결
    m_prodTab = new AdminInfoForm_Prod(this);
    ui->tabWidget->addTab(m_prodTab, tr("제품 정보"));

    // 2. 주문 관리 탭 생성 및 시그널 연결
    m_orderTab = new AdminInfoForm_OL(this);
    ui->tabWidget->addTab(m_orderTab, tr("주문 관리"));

    // 4. 고객사 정보 탭 생성 및 시그널 연결
    m_clientTab = new AdminInfoForm(this);
    ui->tabWidget->addTab(m_clientTab, tr("고객사 정보"));

    // QTabWidget의 시그널 연결
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow_Admin::onTabChanged);
    ui->tabWidget->setCurrentIndex(0); // 기본으로 첫 번째 탭을 보여줌
    onTabChanged(ui->tabWidget->currentIndex());

    // 탭 전환 시 채팅방 알림 아이콘을 지우는 로직
    // connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
    //     QWidget* currentWidget = ui->tabWidget->widget(index);
    //     if (auto chatWidget = qobject_cast<AdminInfoForm_Chat*>(currentWidget)) {
    //         chatWidget->onChatTabActivated();
    //     }
    // });
}

void MainWindow_Admin::onTabChanged(int index)
{
    if (index == 0) {
        m_prodTab->getToolBox()->setCurrentIndex(0);
        m_prodTab->ProductTableSet();
        m_prodTab->handleIncomingData();// 데이터로드
    }else if(index == 1){
        m_orderTab->getToolBox()->setCurrentIndex(0);
        m_orderTab->OrderTableSet();
        m_orderTab->handleIncomingData();
    }else if(index ==2){
        m_clientTab->getToolBox()->setCurrentIndex(0);
        m_clientTab->UserTableSet();
        m_clientTab->handleIncomingData();
    }
}

MainWindow_Admin::~MainWindow_Admin() {
    delete ui;
}

// --- 툴바 액션 슬롯 구현 ---
void MainWindow_Admin::on_actionSecurityProd_Info_triggered() { ui->tabWidget->setCurrentWidget(m_prodTab); }
void MainWindow_Admin::on_actionOrder_Info_triggered() { ui->tabWidget->setCurrentWidget(m_orderTab); }
void MainWindow_Admin::on_actionChatting_Room_triggered() { ui->tabWidget->setCurrentIndex(3); }
void MainWindow_Admin::on_actionClient_Info_triggered() { ui->tabWidget->setCurrentWidget(m_clientTab); }

void MainWindow_Admin::on_actionQuit_triggered() {
    // 소켓 연결 종료
    if (Communication::getInstance()->getSocket() && Communication::getInstance()->getSocket()->state() == QAbstractSocket::ConnectedState) {
        Communication::getInstance()->getSocket()->disconnectFromHost();
        if (Communication::getInstance()->getSocket()->state() != QAbstractSocket::UnconnectedState)
            Communication::getInstance()->getSocket()->waitForDisconnected(1000);
    }
    // 창 닫기
    this->close();
}

void MainWindow_Admin::CreateChatRoom(const QBuffer &buffer)
{
    QSet<QString> uniqueRoomIds;
    QByteArray arr = buffer.data();
    arr.remove(0, buffer.pos());
    QJsonArray UserInfo = QJsonDocument::fromJson(arr).array();
    for (int i = 0; i < UserInfo.size(); ++i) {
        QJsonObject User = UserInfo[i].toObject();
        uniqueRoomIds.insert(User.value("RoomId").toString());
    }
    for (const QString& identifier : uniqueRoomIds) {
        AdminInfoForm_Chat* newChatTab = new AdminInfoForm_Chat(identifier, this);
        int tabIndex = ui->tabWidget->addTab(newChatTab, identifier);
        if(identifier == "Corp"){
            // 수평 레이아웃 가져오기 (채팅창이 있는 레이아웃)
            QHBoxLayout* horizontalLayout = newChatTab->findChild<QHBoxLayout*>("horizontalLayout");

            if(horizontalLayout) {
                // 사용자 목록을 위한 위젯 생성
                QListWidget* userListWidget = new QListWidget(newChatTab);
                userListWidget->setObjectName("userListWidget_" + identifier);
                userListWidget->setMaximumWidth(150); // 적절한 너비 설정

                // 사용자 목록 위젯을 수평 레이아웃에 추가
                horizontalLayout->addWidget(userListWidget);

                // 수직 레이아웃 생성 (버튼을 위한)
                QVBoxLayout* verticalLayout = new QVBoxLayout();

                // 초대 버튼 생성
                QPushButton* inviteButton = new QPushButton("초대", newChatTab);
                inviteButton->setObjectName("inviteButton_" + identifier);

                // 버튼을 수직 레이아웃에 추가
                verticalLayout->addWidget(inviteButton);
                verticalLayout->addStretch(); // 버튼을 상단에 위치시키기 위한 스트레치

                // 수직 레이아웃을 수평 레이아웃에 추가
                horizontalLayout->addLayout(verticalLayout);

                // 사용자 선택 및 버튼 클릭 이벤트 연결
                connect(inviteButton, &QPushButton::clicked, [=]() {
                    QListWidgetItem* selectedItem = userListWidget->currentItem();
                    if(selectedItem) {
                        QString selectedUser = selectedItem->text();
                        qDebug() << "특정 사용자 초대: " << selectedUser;
                        emit InviteUser(selectedUser);
                    } else {
                        qDebug() << "선택된 사용자가 없습니다.";
                    }
                });
                // 예시 사용자 추가 (실제로는 서버에서 받아온 사용자 목록을 추가해야 함)
                userListWidget->addItem("사용자1");
                userListWidget->addItem("사용자2");
                userListWidget->addItem("사용자3");
            }

        }

        qDebug() << "탭 추가됨: " << identifier << " (인덱스: " << tabIndex << ")";
    }
}

// 새로운 채팅방 탭 생성 (회사별 채팅방)
void MainWindow_Admin::createNewChatTab(const QString& roomId, const QString& initialMessage) {
    // // 새 채팅방 탭 생성
    // AdminInfoForm_Chat* newChatTab = new AdminInfoForm_Chat(roomId, this);
    // connect(newChatTab, &AdminInfoForm_Chat::messageSendRequested,
    //         this, &MainWindow_Admin::sendChatMessage);

    // // 탭 추가 (탭 이름은 회사명)
    // ui->tabWidget->addTab(newChatTab, roomId);

    // // 채팅방 탭 맵에 추가
    // m_chatTabs[roomId] = newChatTab;

    // // 초기 메시지 표시
    // if (!initialMessage.isEmpty()) {
    //     newChatTab->appendMessage(initialMessage);
    // }

    // // 새 메시지가 왔으므로 알림 표시
    // newChatTab->showChatNotification();

    // // 채팅 기록 요청
    // QJsonObject requestObj;
    // requestObj["roomId"] = roomId;
    // sendDataToServer(Protocol::Chat_History_Request, requestObj, "chat_history");
}
