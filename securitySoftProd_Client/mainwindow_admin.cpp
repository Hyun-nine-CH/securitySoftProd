#include "mainwindow_admin.h"
#include "ui_mainwindow_admin.h"
#include "admininfoform.h"
#include "admininfoform_prod.h"
#include "admininfoform_ol.h"
#include "admininfoform_chat.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>

// 클라이언트-서버 간 통신 프로토콜 정의
namespace Protocol {
enum DataType : qint64 {
    Chat_Message = 0x08,
    Request_Product_List = 0x09, Response_Product_List = 0x0A,
    Request_Member_List = 0x0B, Response_Member_List = 0x0C,
    Request_Order_List = 0x0D, Response_Order_List = 0x0E,
    // ... 다른 프로토콜 타입들 ...
};
}

MainWindow_Admin::MainWindow_Admin(QTcpSocket* socket, const QJsonObject& userInfo, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow_Admin), m_socket(socket), m_userInfo(userInfo)
{
    ui->setupUi(this);
    // 서버로부터 데이터가 오면 handleServerMessage 슬롯이 호출되도록 연결
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow_Admin::handleServerMessage);

    // 툴바 액션(아이콘 버튼)과 탭 전환 슬롯 연결
    connect(ui->actionClient_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionClient_Info_triggered);
    connect(ui->actionSecurityProd_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionSecurityProd_Info_triggered);
    connect(ui->actionOrder_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionOrder_Info_triggered);
    connect(ui->actionChatting_Room, &QAction::triggered, this, &MainWindow_Admin::on_actionChatting_Room_triggered);

    // 1. 회원 관리 탭 생성 및 시그널 연결
    m_memberTab = new AdminInfoForm(this);
    connect(m_memberTab, &AdminInfoForm::memberListRequested, this, &MainWindow_Admin::requestMemberList);
    connect(m_memberTab, &AdminInfoForm::searchMembersRequested, this, &MainWindow_Admin::searchMembers);
    ui->tabWidget->addTab(m_memberTab, tr("회원 관리"));

    // 2. 제품 관리 탭 생성 및 시그널 연결
    m_prodTab = new AdminInfoForm_Prod(this);
    connect(m_prodTab, &AdminInfoForm_Prod::productListRequested, this, &MainWindow_Admin::requestProductList);
    // ... (제품 추가/수정/삭제 시그널도 여기에 연결) ...
    ui->tabWidget->addTab(m_prodTab, tr("제품 관리"));

    // 3. 주문 관리 탭 생성 및 시그널 연결
    m_orderTab = new AdminInfoForm_OL(this);
    connect(m_orderTab, &AdminInfoForm_OL::orderListRequested, this, &MainWindow_Admin::requestOrderList);
    connect(m_orderTab, &AdminInfoForm_OL::searchOrdersRequested, this, &MainWindow_Admin::searchOrders);
    ui->tabWidget->addTab(m_orderTab, tr("주문 관리"));

    // 4. 관리자 전용 채팅방 기본 생성
    createOrSwitchToChatTab(ADMIN_CHAT_ROOM_ID);

    // 탭 전환 시 채팅방 알림 아이콘을 지우는 로직
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (auto chatWidget = qobject_cast<AdminInfoForm_Chat*>(ui->tabWidget->widget(index))) {
            chatWidget->onChatTabActivated();
        }
    });
}

MainWindow_Admin::~MainWindow_Admin() { delete ui; }

// --- 툴바 액션 슬롯 구현 ---
void MainWindow_Admin::on_actionClient_Info_triggered() { ui->tabWidget->setCurrentWidget(m_memberTab); }
void MainWindow_Admin::on_actionSecurityProd_Info_triggered() { ui->tabWidget->setCurrentWidget(m_prodTab); }
void MainWindow_Admin::on_actionOrder_Info_triggered() { ui->tabWidget->setCurrentWidget(m_orderTab); }
void MainWindow_Admin::on_actionChatting_Room_triggered() {
    // 채팅방은 여러 개일 수 있으므로 첫 번째 채팅방(관리자 채팅방)으로 이동
    if (m_chatTabs.contains(ADMIN_CHAT_ROOM_ID))
        ui->tabWidget->setCurrentWidget(m_chatTabs.value(ADMIN_CHAT_ROOM_ID));
}

// --- 자식 위젯 요청 처리 슬롯들 ---
void MainWindow_Admin::requestMemberList() { sendDataToServer(Protocol::Request_Member_List, {}, "req_members"); }
void MainWindow_Admin::searchMembers(const QString& c, const QString& d, const QString& p) {
    QJsonObject searchData; searchData["company"] = c; searchData["department"] = d; searchData["phone"] = p;
    sendDataToServer(Protocol::Request_Member_List, searchData, "search_members");
}
void MainWindow_Admin::requestOrderList() { sendDataToServer(Protocol::Request_Order_List, {}, "req_orders"); }
void MainWindow_Admin::searchOrders(const QString& pn, const QString& dd) {
    QJsonObject searchData; searchData["productName"] = pn; searchData["dueDate"] = dd;
    sendDataToServer(Protocol::Request_Order_List, searchData, "search_orders");
}
void MainWindow_Admin::requestProductList() { sendDataToServer(Protocol::Request_Product_List, {}, "req_prods"); }

void MainWindow_Admin::sendChatMessage(const QString& companyName, const QString& message) {
    QString nickName = QString("[%1][%2][%3]")
    .arg(m_userInfo["RoomId"].toString()) // "Corp"
        .arg(m_userInfo["department"].toString())
        .arg(m_userInfo["manager"].toString());

    QJsonObject chatObject;
    chatObject["nickName"] = nickName;
    chatObject["message"] = message;

    // 내가 보낸 메시지는 해당 탭에 바로 표시
    if (m_chatTabs.contains(companyName)) {
        m_chatTabs.value(companyName)->appendMessage(nickName + ": " + message);
    }

    sendDataToServer(Protocol::Chat_Message, chatObject, companyName); // companyName이 Target RoomId
}

// --- 서버 통신 ---
void MainWindow_Admin::sendDataToServer(qint64 dataType, const QJsonObject& payload, const QString& filename)
{
    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << qint64(0) << qint64(0) << qint64(0) << filename;
    if (!payload.isEmpty()) {
        blockToSend.append(QJsonDocument(payload).toJson());
    }
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << dataType << totalSize << totalSize;
    m_socket->write(blockToSend);
    m_socket->flush();
}

void MainWindow_Admin::handleServerMessage() {
    m_buffer.append(m_socket->readAll());
    while(true) {
        QDataStream in(&m_buffer, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_15);
        if (m_buffer.size() < (3 * sizeof(qint64))) break;
        qint64 dataType, totalSize, currentPacketSize;
        in >> dataType >> totalSize >> currentPacketSize;
        if (m_buffer.size() < totalSize) break;
        QString filename; // RoomId가 담겨 올 필드
        in >> filename;
        QByteArray payload = m_buffer.mid(in.device()->pos(), totalSize - in.device()->pos());
        QJsonDocument doc = QJsonDocument::fromJson(payload);

        if (dataType == Protocol::Chat_Message) {
            if (doc.isObject()) {
                QJsonObject chatObj = doc.object();
                QString formattedMessage = chatObj["nickName"].toString() + ": " + chatObj["message"].toString();
                createOrSwitchToChatTab(filename); // filename is the roomId
                if (m_chatTabs.contains(filename)) {
                    m_chatTabs.value(filename)->appendMessage(formattedMessage);
                }
            }
        } else if (dataType == Protocol::Response_Product_List) {
            if (doc.isArray()) m_prodTab->displayProductList(doc.array());
        } else if (dataType == Protocol::Response_Member_List) {
            if (doc.isArray()) m_memberTab->displayMemberList(doc.array());
        } else if (dataType == Protocol::Response_Order_List) {
            if (doc.isArray()) m_orderTab->displayOrderList(doc.array());
        }
        m_buffer.remove(0, totalSize);
    }
}

void MainWindow_Admin::createOrSwitchToChatTab(const QString& companyName)
{
    if (m_chatTabs.contains(companyName)) return;

    AdminInfoForm_Chat* chatTab = new AdminInfoForm_Chat(companyName, this);
    connect(chatTab, &AdminInfoForm_Chat::messageSendRequested, this, &MainWindow_Admin::sendChatMessage);
    m_chatTabs.insert(companyName, chatTab);
    QString tabName = (companyName == ADMIN_CHAT_ROOM_ID) ? tr("관리자 채팅") : QString("%1 채팅").arg(companyName);
    ui->tabWidget->addTab(chatTab, tabName);
}
