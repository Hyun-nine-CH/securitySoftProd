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
#include "Protocol.h"

MainWindow_Admin::MainWindow_Admin(QTcpSocket* socket, const QJsonObject& userInfo, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow_Admin), m_socket(socket), m_userInfo(userInfo)
{
    ui->setupUi(this);

    // 윈도우 제목 설정 - 관리자 정보 표시
    QString title = QString("관리자 - %1 (%2)")
                        .arg(m_userInfo["department"].toString())
                        .arg(m_userInfo["manager"].toString());
    this->setWindowTitle(title);

    // 서버로부터 데이터가 오면 handleServerMessage 슬롯이 호출되도록 연결
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow_Admin::handleServerMessage);

    // 툴바 액션(아이콘 버튼)과 탭 전환 슬롯 연결
    connect(ui->actionSecurityProd_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionSecurityProd_Info_triggered);
    connect(ui->actionOrder_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionOrder_Info_triggered);
    connect(ui->actionChatting_Room, &QAction::triggered, this, &MainWindow_Admin::on_actionChatting_Room_triggered);
    connect(ui->actionClient_Info, &QAction::triggered, this, &MainWindow_Admin::on_actionClient_Info_triggered);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow_Admin::on_actionQuit_triggered);

    // 1. 제품 정보 탭 생성 및 시그널 연결
    m_prodTab = new AdminInfoForm_Prod(this);
    connect(m_prodTab, &AdminInfoForm_Prod::productListRequested, this, &MainWindow_Admin::requestProductList);
    ui->tabWidget->addTab(m_prodTab, tr("제품 정보"));

    // 2. 주문 관리 탭 생성 및 시그널 연결
    m_orderTab = new AdminInfoForm_OL(this);
    connect(m_orderTab, &AdminInfoForm_OL::orderListRequested, this, &MainWindow_Admin::requestOrderList);
    ui->tabWidget->addTab(m_orderTab, tr("주문 관리"));

    // 3. 기본 채팅방 탭 생성 (관리자 전용 채팅방)
    m_adminChatTab = new AdminInfoForm_Chat("Corp", this); // "Corp"는 관리자 전용 채팅방 ID
    connect(m_adminChatTab, &AdminInfoForm_Chat::messageSendRequested,
            this, &MainWindow_Admin::sendChatMessage);
    ui->tabWidget->addTab(m_adminChatTab, tr("관리자 채팅"));

    // 채팅방 탭 맵에 추가 (채팅방 ID를 키로 사용)
    m_chatTabs["Corp"] = m_adminChatTab;

    // 4. 고객사 정보 탭 생성 및 시그널 연결
    m_clientTab = new AdminInfoForm(this);
    connect(m_clientTab, &AdminInfoForm::clientListRequested, this, &MainWindow_Admin::requestClientList);
    ui->tabWidget->addTab(m_clientTab, tr("고객사 정보"));

    ui->tabWidget->setCurrentIndex(0); // 기본으로 첫 번째 탭을 보여줌

    // 탭 전환 시 채팅방 알림 아이콘을 지우는 로직
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        QWidget* currentWidget = ui->tabWidget->widget(index);
        if (auto chatWidget = qobject_cast<AdminInfoForm_Chat*>(currentWidget)) {
            chatWidget->onChatTabActivated();
        }
    });

    // 초기 데이터 요청 (500ms 후)
    QTimer::singleShot(500, this, &MainWindow_Admin::requestProductList);
    QTimer::singleShot(700, this, &MainWindow_Admin::requestClientList);
    QTimer::singleShot(900, this, &MainWindow_Admin::requestOrderList);
}

MainWindow_Admin::~MainWindow_Admin() {
    delete ui;
}

// --- 툴바 액션 슬롯 구현 ---
void MainWindow_Admin::on_actionSecurityProd_Info_triggered() { ui->tabWidget->setCurrentWidget(m_prodTab); }
void MainWindow_Admin::on_actionOrder_Info_triggered() { ui->tabWidget->setCurrentWidget(m_orderTab); }
void MainWindow_Admin::on_actionChatting_Room_triggered() { ui->tabWidget->setCurrentWidget(m_adminChatTab); }
void MainWindow_Admin::on_actionClient_Info_triggered() { ui->tabWidget->setCurrentWidget(m_clientTab); }

void MainWindow_Admin::on_actionQuit_triggered() {
    // 서버에 로그아웃 메시지 전송
    QJsonObject logoutData;
    logoutData["action"] = "logout";
    logoutData["userId"] = m_userInfo["id"].toString();

    // 로그아웃 요청 전송
    sendDataToServer(Protocol::Logout_Request, logoutData, "logout");

    // 소켓 연결 종료
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState)
            m_socket->waitForDisconnected(1000);
    }

    // 창 닫기
    this->close();
}
// --- 자식 위젯 요청 처리 슬롯들 ---
void MainWindow_Admin::requestProductList() {
    sendDataToServer(Protocol::Request_Product_List, {}, "req_prods");
}

void MainWindow_Admin::requestClientList() {
    sendDataToServer(Protocol::Request_Client_List, {}, "req_clients");
}

void MainWindow_Admin::requestOrderList() {
    sendDataToServer(Protocol::Request_Order_List, {}, "req_orders");
}

void MainWindow_Admin::sendChatMessage(const QString& roomId, const QString& message) {
    // 관리자 닉네임 생성 (회사명은 "Corp"로 고정)
    QString nickName = QString("[%1][%2][%3]")
                           .arg("Corp") // 관리자는 항상 "Corp" 소속
                           .arg(m_userInfo["department"].toString())
                           .arg(m_userInfo["manager"].toString());

    QJsonObject chatObject;
    chatObject["nickName"] = nickName;
    chatObject["message"] = message;
    chatObject["roomId"] = roomId; // 어느 채팅방에 메시지를 보낼지 지정

    // 내가 보낸 메시지는 해당 채팅방 탭에 바로 표시
    if (m_chatTabs.contains(roomId)) {
        m_chatTabs[roomId]->appendMessage(nickName + ": " + message);
    }

    // 서버로 전송
    sendDataToServer(Protocol::Chat_Message, chatObject, roomId);
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

        QString filename;
        in >> filename;
        QByteArray payload = m_buffer.mid(in.device()->pos(), totalSize - in.device()->pos());

        // 데이터 타입에 따라 처리
        if (dataType == Protocol::Chat_Message) {
            QJsonDocument doc = QJsonDocument::fromJson(payload);
            if (doc.isObject()) {
                QJsonObject chatObj = doc.object();
                QString roomId = chatObj["roomId"].toString();

                // 해당 roomId의 채팅방 탭이 있는지 확인
                if (m_chatTabs.contains(roomId)) {
                    // 기존 채팅방 탭에 데이터 전달
                    m_chatTabs[roomId]->handleIncomingData(dataType, payload, filename);

                    // 현재 활성화된 탭이 아니면 알림 표시
                    if (ui->tabWidget->currentWidget() != m_chatTabs[roomId]) {
                        m_chatTabs[roomId]->showChatNotification();
                    }
                } else {
                    // 새로운 채팅방 탭 생성 (회사별 채팅방)
                    createNewChatTab(roomId);
                    // 새로 생성된 탭에 데이터 전달
                    m_chatTabs[roomId]->handleIncomingData(dataType, payload, filename);
                }
            }
        } else if (dataType == Protocol::Chat_History_Response) {
            // 채팅 기록 응답도 해당 채팅방 탭으로 전달
            QJsonDocument doc = QJsonDocument::fromJson(payload);
            if (doc.isObject()) {
                QJsonObject historyObj = doc.object();
                QString roomId = historyObj["roomId"].toString();

                if (m_chatTabs.contains(roomId)) {
                    m_chatTabs[roomId]->handleIncomingData(dataType, payload, filename);
                }
            }
        } else if (dataType == Protocol::Response_Product_List ||
                   dataType == Protocol::Add_Product ||
                   dataType == Protocol::Delete_Product ||
                   dataType == Protocol::Update_Product) {

            m_prodTab->handleIncomingData(dataType, payload, filename);
//            QJsonDocument doc = QJsonDocument::fromJson(payload);
//            if (doc.isArray()) {
//                m_prodTab->displayProductList(doc.array());
//            }
        } else if (dataType == Protocol::Response_Client_List) {
            QJsonDocument doc = QJsonDocument::fromJson(payload);
            if (doc.isArray()) {
                m_clientTab->displayClientList(doc.array());
            }
        } else if (dataType == Protocol::Response_Order_List) {

            m_orderTab->handleIncomingData(dataType, payload, filename);
//          QJsonDocument doc = QJsonDocument::fromJson(payload);
//          if (doc.isArray()) {
//              m_orderTab->displayOrderList(doc.array());
//          }
        } else if (dataType == Protocol::Chat_History_Response) {
            QJsonDocument doc = QJsonDocument::fromJson(payload);
            if (doc.isObject()) {
                QJsonObject historyObj = doc.object();
                QString roomId = historyObj["roomId"].toString();
                QJsonArray messages = historyObj["messages"].toArray();

                // 해당 roomId의 채팅방 탭이 있는지 확인
                if (m_chatTabs.contains(roomId)) {
                    // 채팅 기록 표시
                    for (const QJsonValue &value : messages) {
                        QJsonObject msgObj = value.toObject();
                        QString formattedMessage = msgObj["nickName"].toString() + ": " + msgObj["message"].toString();
                        m_chatTabs[roomId]->appendMessage(formattedMessage);
                    }
                    m_chatTabs[roomId]->appendMessage("--- 채팅 기록 로드 완료 ---");
                }
            }
        }

        m_buffer.remove(0, totalSize);
    }
}

// 새로운 채팅방 탭 생성 (회사별 채팅방)
void MainWindow_Admin::createNewChatTab(const QString& roomId, const QString& initialMessage) {
    // 새 채팅방 탭 생성
    AdminInfoForm_Chat* newChatTab = new AdminInfoForm_Chat(roomId, this);
    connect(newChatTab, &AdminInfoForm_Chat::messageSendRequested,
            this, &MainWindow_Admin::sendChatMessage);

    // 탭 추가 (탭 이름은 회사명)
    ui->tabWidget->addTab(newChatTab, roomId);

    // 채팅방 탭 맵에 추가
    m_chatTabs[roomId] = newChatTab;

    // 초기 메시지 표시
    if (!initialMessage.isEmpty()) {
        newChatTab->appendMessage(initialMessage);
    }

    // 새 메시지가 왔으므로 알림 표시
    newChatTab->showChatNotification();

    // 채팅 기록 요청
    QJsonObject requestObj;
    requestObj["roomId"] = roomId;
    sendDataToServer(Protocol::Chat_History_Request, requestObj, "chat_history");
}
