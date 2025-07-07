#include "mainwindow_admin.h"
#include "ui_mainwindow_admin.h"
#include "admininfoform_prod.h"
#include "admininfoform.h"
#include "admininfoform_chat.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>

namespace Protocol {
enum DataType : qint64 {
    Chat_Message = 0x08,
};
}

MainWindow_Admin::MainWindow_Admin(QTcpSocket* socket, qint64 clientId, const QString& managerName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow_Admin)
    , m_socket(socket)
    , m_clientId(clientId)
    , m_managerName(managerName)
{
    ui->setupUi(this);

    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow_Admin::handleServerMessage);

    // 고객 관리, 제품 관리 탭 생성
    AdminInfoForm* clientTab = new AdminInfoForm(this);
    ui->tabWidget->addTab(clientTab, tr("고객 관리"));
    AdminInfoForm_Prod* prodTab = new AdminInfoForm_Prod(this);
    ui->tabWidget->addTab(prodTab, tr("제품 관리"));

    // 관리자 로그인 시, 관리자 전용 채팅방을 기본으로 생성합니다.
    createOrSwitchToChatTab(ADMIN_CHAT_ROOM_ID);

    // 탭 전환 시 알림 아이콘 제거 기능
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (auto chatWidget = qobject_cast<AdminInfoForm_Chat*>(ui->tabWidget->widget(index))) {
            chatWidget->onChatTabActivated();
        }
    });
}

MainWindow_Admin::~MainWindow_Admin()
{
    delete ui;
}

// 서버로부터 온 메시지를 받아 올바른 탭에 분배
void MainWindow_Admin::handleServerMessage()
{
    m_buffer.append(m_socket->readAll());

    while(true) {
        QDataStream in(&m_buffer, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_15);

        if (m_buffer.size() < (3 * sizeof(qint64))) break;

        qint64 dataType, totalSize, currentPacketSize;
        in >> dataType >> totalSize >> currentPacketSize;

        if (m_buffer.size() < totalSize) break;

        if (dataType == Protocol::Chat_Message) {
            QString roomId; // 메시지가 온 회사 이름 또는 "@Admins"
            in >> roomId;
            QByteArray payload = m_buffer.mid(in.device()->pos(), totalSize - in.device()->pos());
            QJsonDocument doc = QJsonDocument::fromJson(payload);

            if (doc.isObject()) {
                QJsonObject chatObj = doc.object();
                QString senderName = chatObj["senderName"].toString();
                QString message = chatObj["message"].toString();

                // 해당 채팅 탭을 찾거나 새로 생성
                createOrSwitchToChatTab(roomId);
                if (m_chatTabs.contains(roomId)) {
                    QString formattedMessage = QString("[%1]: %2").arg(senderName, message);
                    m_chatTabs.value(roomId)->appendMessage(formattedMessage);
                }
            }
        }
        m_buffer.remove(0, totalSize);
    }
}

// 특정 회사 또는 관리자 채팅방에 메시지 전송
void MainWindow_Admin::sendChatMessage(const QString& companyName, const QString& message)
{
    QJsonObject chatObject;
    chatObject["senderName"] = m_managerName;
    chatObject["message"] = message;
    QByteArray payload = QJsonDocument(chatObject).toJson();

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << qint64(0) << qint64(0) << qint64(0) << companyName; // companyName은 RoomId 역할
    blockToSend.append(payload);

    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Chat_Message) << totalSize << totalSize;

    if (m_chatTabs.contains(companyName)) {
        m_chatTabs.value(companyName)->appendMessage(QString("[%1]: %2").arg(m_managerName, message));
    }

    m_socket->write(blockToSend);
    m_socket->flush();
}

// 채팅 탭을 동적으로 생성하거나 기존 탭으로 전환하는 함수
void MainWindow_Admin::createOrSwitchToChatTab(const QString& companyName)
{
    // 이미 탭이 존재하면 아무것도 하지 않음
    if (m_chatTabs.contains(companyName)) {
        return;
    }

    // 새 채팅 탭 생성
    AdminInfoForm_Chat* chatTab = new AdminInfoForm_Chat(companyName, this);

    // 채팅 탭의 '전송 요청' 시그널을 MainWindow_Admin의 '전송' 슬롯에 연결
    connect(chatTab, &AdminInfoForm_Chat::messageSendRequested, this, &MainWindow_Admin::sendChatMessage);

    m_chatTabs.insert(companyName, chatTab);

    // 관리자 채팅방일 경우 탭 이름을 다르게 설정
    QString tabName = (companyName == ADMIN_CHAT_ROOM_ID) ? tr("관리자 채팅") : QString("%1 채팅").arg(companyName);
    ui->tabWidget->addTab(chatTab, tabName);
}
