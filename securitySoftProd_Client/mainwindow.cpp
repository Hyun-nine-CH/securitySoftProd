#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientinfoform_prod.h"
#include "clientinfoform.h"
#include "clientinfoform_chat.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>

namespace Protocol {
enum DataType : qint64 {
    Chat_Message = 0x08,
};
}

MainWindow::MainWindow(QTcpSocket* socket, const QString& roomId, qint64 clientId, const QString& managerName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_socket(socket)
    , m_roomId(roomId)
    , m_clientId(clientId)
    , m_managerName(managerName)
{
    ui->setupUi(this);

    // 소켓의 readyRead 시그널을 이 창의 핸들러에 연결
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::handleServerMessage);

    // 제품 정보, 주문 정보 탭 생성 (기존 로직)
    ClientInfoForm_Prod* clientProd = new ClientInfoForm_Prod(this);
    ui->tabWidget->addTab(clientProd, tr("제품 정보"));
    ClientInfoForm* clientInfo = new ClientInfoForm(this);
    ui->tabWidget->addTab(clientInfo, tr("주문 정보"));

    // 채팅 탭 생성 및 시그널 연결
    m_chatTab = new ClientInfoForm_Chat(this);
    connect(m_chatTab, &ClientInfoForm_Chat::messageSendRequested, this, &MainWindow::sendChatMessage);
    ui->tabWidget->addTab(m_chatTab, tr("채팅방"));

    ui->tabWidget->setCurrentIndex(0);

    // 탭 전환 시 알림 제거 기능
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (auto chatWidget = qobject_cast<ClientInfoForm_Chat*>(ui->tabWidget->widget(index))) {
            chatWidget->onChatTabActivated();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 서버로부터 온 채팅 메시지 처리
void MainWindow::handleServerMessage()
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
            QString filename; // roomId가 담겨 올 필드
            in >> filename;
            QByteArray payload;
            payload = m_buffer.mid(in.device()->pos(), totalSize - in.device()->pos());
            QJsonDocument doc = QJsonDocument::fromJson(payload);

            if (doc.isObject()) {
                QJsonObject chatObj = doc.object();
                QString senderName = chatObj["senderName"].toString();
                QString message = chatObj["message"].toString();

                // 내 회사(RoomId) 채팅방의 메시지만 표시
                if (filename == m_roomId) {
                    QString formattedMessage = QString("[%1]: %2").arg(senderName, message);
                    m_chatTab->appendMessage(formattedMessage);
                }
            }
        }
        m_buffer.remove(0, totalSize);
    }
}

// 채팅 탭의 요청을 받아 서버로 메시지 전송
void MainWindow::sendChatMessage(const QString& message)
{
    // 내가 보낸다는 정보(이름)를 JSON에 담음
    QJsonObject chatObject;
    chatObject["senderName"] = m_managerName;
    chatObject["message"] = message;
    QByteArray payload = QJsonDocument(chatObject).toJson();

    // 서버에 보낼 최종 데이터 블록 생성
    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    // [타입][크기][크기][파일이름(RoomId)][JSON Payload]
    out << qint64(0) << qint64(0) << qint64(0) << m_roomId;
    blockToSend.append(payload);

    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Chat_Message) << totalSize << totalSize;

    // 내가 보낸 메시지는 로컬에 바로 표시
    m_chatTab->appendMessage(QString("[%1]: %2").arg(m_managerName, message));

    m_socket->write(blockToSend);
    m_socket->flush();
}
