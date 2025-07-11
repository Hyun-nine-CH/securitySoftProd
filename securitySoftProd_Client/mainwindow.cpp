#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientinfoform_prod.h"
#include "clientinfoform.h"
#include "clientinfoform_chat.h"
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
    Submit_Order = 0x0F,
    // ... 다른 프로토콜 타입들 ...
};
}

MainWindow::MainWindow(QTcpSocket* socket, const QJsonObject& userInfo, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_socket(socket), m_userInfo(userInfo)
{
    ui->setupUi(this);
    // 서버로부터 데이터가 오면 handleServerMessage 슬롯이 호출되도록 연결
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::handleServerMessage);

    // 툴바 액션(아이콘 버튼)과 탭 전환 슬롯 연결
    connect(ui->actionSecurityProd_Info, &QAction::triggered, this, &MainWindow::on_actionSecurityProd_Info_triggered);
    connect(ui->actionOrder_Info, &QAction::triggered, this, &MainWindow::on_actionOrder_Info_triggered);
    connect(ui->actionChatting_Room, &QAction::triggered, this, &MainWindow::on_actionChatting_Room_triggered);

    // 1. 제품 정보 탭 생성 및 시그널 연결
    m_prodTab = new ClientInfoForm_Prod(this);
    connect(m_prodTab, &ClientInfoForm_Prod::productListRequested, this, &MainWindow::requestProductList);
    connect(m_prodTab, &ClientInfoForm_Prod::searchProductsRequested, this, &MainWindow::searchProducts);
    ui->tabWidget->addTab(m_prodTab, tr("제품 정보"));

    // 2. 주문 하기 탭 생성 및 시그널 연결
    m_orderTab = new ClientInfoForm(this);
    connect(m_orderTab, &ClientInfoForm::orderSubmitted, this, &MainWindow::submitOrder);
    ui->tabWidget->addTab(m_orderTab, tr("주문 하기"));

    // 3. 채팅방 탭 생성 및 시그널 연결
    m_chatTab = new ClientInfoForm_Chat(this);
    connect(m_chatTab, &ClientInfoForm_Chat::messageSendRequested, this, &MainWindow::sendChatMessage);
    ui->tabWidget->addTab(m_chatTab, tr("채팅방"));

    ui->tabWidget->setCurrentIndex(0); // 기본으로 첫 번째 탭을 보여줌

    // 탭 전환 시 채팅방 알림 아이콘을 지우는 로직
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (auto chatWidget = qobject_cast<ClientInfoForm_Chat*>(ui->tabWidget->widget(index))) {
            chatWidget->onChatTabActivated();
        }
    });
}

MainWindow::~MainWindow() { delete ui; }

// --- 툴바 액션 슬롯 구현 ---
void MainWindow::on_actionSecurityProd_Info_triggered() { ui->tabWidget->setCurrentWidget(m_prodTab); }
void MainWindow::on_actionOrder_Info_triggered() { ui->tabWidget->setCurrentWidget(m_orderTab); }
void MainWindow::on_actionChatting_Room_triggered() { ui->tabWidget->setCurrentWidget(m_chatTab); }

// --- 자식 위젯 요청 처리 슬롯들 ---
void MainWindow::requestProductList() {
    sendDataToServer(Protocol::Request_Product_List, {}, "req_prods");
}

void MainWindow::searchProducts(const QString& name, const QString& price, const QString& dueDate) {
    QJsonObject searchData;
    searchData["name"] = name;
    searchData["price"] = price;
    searchData["dueDate"] = dueDate;
    sendDataToServer(Protocol::Request_Product_List, searchData, "search_prods");
}

void MainWindow::submitOrder(const QJsonObject& orderData) {
    sendDataToServer(Protocol::Submit_Order, orderData, "submit_order");
    QMessageBox::information(this, "주문 완료", "주문이 성공적으로 접수되었습니다.");
}

void MainWindow::sendChatMessage(const QString& message) {
    QString nickName = QString("[%1][%2][%3]")
    .arg(m_userInfo["RoomId"].toString())
        .arg(m_userInfo["department"].toString())
        .arg(m_userInfo["manager"].toString());

    QJsonObject chatObject;
    chatObject["nickName"] = nickName;
    chatObject["message"] = message;

    // 내가 보낸 메시지는 로컬에 바로 표시
    m_chatTab->appendMessage(nickName + ": " + message);

    sendDataToServer(Protocol::Chat_Message, chatObject, m_userInfo["RoomId"].toString());
}

// --- 서버 통신 ---
void MainWindow::sendDataToServer(qint64 dataType, const QJsonObject& payload, const QString& filename)
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

void MainWindow::handleServerMessage() {
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

        if (dataType == Protocol::Chat_Message) {
            QJsonDocument doc = QJsonDocument::fromJson(payload);
            if (doc.isObject()) {
                QJsonObject chatObj = doc.object();
                QString formattedMessage = chatObj["nickName"].toString() + ": " + chatObj["message"].toString();
                m_chatTab->appendMessage(formattedMessage);
            }
        } else if (dataType == Protocol::Response_Product_List) {
            QJsonDocument doc = QJsonDocument::fromJson(payload);
            if (doc.isArray()) {
                m_prodTab->displayProductList(doc.array());
            }
        }
        // ... 다른 데이터 타입(주문 결과 등)에 대한 처리 ...
        m_buffer.remove(0, totalSize);
    }
}
