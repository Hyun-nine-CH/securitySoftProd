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
    connect(this, &MainWindow_Admin::InviteUser,Communication::getInstance(), &Communication::RequestInviteUser); //채팅초대
    connect(Communication::getInstance(), &Communication::ReceiveActiveUserList, this, &MainWindow_Admin::ReceiveActiveUserList);

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
                QListWidget* userListWidget = new QListWidget(newChatTab);
                // 사용자 목록을 위한 위젯 생성
                userListWidget->setObjectName("userListWidget_" + identifier);
                userListWidget->setMaximumWidth(150); // 적절한 너비 설정
                newChatTab->setUserListWidget(userListWidget);
                // 수직 레이아웃 생성 (버튼을 위한)
                QVBoxLayout* verticalLayout = new QVBoxLayout();

                // 초대 버튼 생성
                QPushButton* inviteButton = new QPushButton("초대", newChatTab);
                inviteButton->setObjectName("inviteButton_" + identifier);

                // 버튼을 수직 레이아웃에 추가
                verticalLayout->addWidget(userListWidget);
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
            }

        }
        qDebug() << "탭 추가됨: " << identifier << " (인덱스: " << tabIndex << ")";
    }
    connect(ui->tabWidget, &QTabWidget::currentChanged,this, &MainWindow_Admin::RequestActiveUserList);
}

void MainWindow_Admin::RequestActiveUserList(int index)
{
    // 현재 선택된 탭의 텍스트(이름)를 가져옵니다.
    QString tabName = ui->tabWidget->tabText(index);

    qDebug() << "탭이 변경되었습니다. 현재 탭 이름:" << tabName << ", 인덱스:" << index;

    // 탭 이름이 "Corp"일 경우에만 특정 동작 수행
    if (tabName == "Corp") {
        Communication::getInstance()->RequestActiveUserList();
    }
}

void MainWindow_Admin::ReceiveActiveUserList(const QBuffer &buffer)
{
    // 현재 활성화된 탭 가져오기
    int currentIndex = ui->tabWidget->currentIndex();
    if (currentIndex < 0) return; // 탭이 없는 경우

    AdminInfoForm_Chat* currentTab = qobject_cast<AdminInfoForm_Chat*>(ui->tabWidget->widget(currentIndex));
    if (!currentTab) return;

    // 현재 탭의 QListWidget 가져오기
    QListWidget* currentListWidget = currentTab->getUserListWidget();
    if (!currentListWidget) return;

    // 기존 항목 지우기
    currentListWidget->clear();

    QByteArray arr = buffer.data();
    arr.remove(0, buffer.pos());
    qDebug() << "이건또 왜 다르니 "<< arr;
    QJsonArray ActiveUser = QJsonDocument::fromJson(arr).array();

    qDebug() << "받은 사용자 목록 크기: " << ActiveUser.size();

    // QListWidget에 ActiveUser 배열의 항목들을 추가
    for (int i = 0; i < ActiveUser.size(); ++i) {
        QJsonObject userObj = ActiveUser[i].toObject();

        QString displayText;
        if (userObj.contains("id")) {
            displayText = userObj["id"].toString();
            if(QString::compare(displayText, Communication::getInstance()->getUserInfo().value("id").toString()) == 0)
                displayText = "Me";
            qDebug() << "사용자 추가: " << displayText;
        } else {
            displayText = "현재 사용자가 없습니다";
            qDebug() << "사용자 정보 없음";
        }

        // QListWidget에 항목 추가
        currentListWidget->addItem(displayText);
    }

    qDebug() << "위젯에 추가된 항목 수: " << currentListWidget->count();
}
