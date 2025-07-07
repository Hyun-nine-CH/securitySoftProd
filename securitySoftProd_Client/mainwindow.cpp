#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientinfoform_prod.h"
#include "clientinfoform.h"
#include "clientinfoform_chat.h"
#include <QDataStream>

// ⭐️ 데이터 타입 정의 (관리자와 동일한 프로토콜 사용)
namespace Protocol {
enum DataType : qint64 {
    Chat_Message = 0x01,
    // 다른 데이터 타입 추가 가능
};
}

MainWindow::MainWindow(QTcpSocket* socket, const QString& companyName, qint64 clientId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_socket(socket)
    , m_companyName(companyName) // ⭐️ 회사 이름(RoomId) 저장
    , m_myUserId(clientId)       // ⭐️ 클라이언트 ID 저장
{
    ui->setupUi(this);

    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::handleServerMessage);

    // 제품 정보, 주문 정보 탭 생성
    ClientInfoForm_Prod* clientProd = new ClientInfoForm_Prod(this);
    ui->tabWidget->addTab(clientProd, tr("제품 정보"));
    ClientInfoForm* clientInfo = new ClientInfoForm(this);
    ui->tabWidget->addTab(clientInfo, tr("주문 정보"));

    // 채팅 탭 생성 및 시그널 연결
    m_chatTab = new ClientInfoForm_Chat(this);
    // ⭐️ 채팅 탭의 '전송 요청' 시그널을 MainWindow의 '전송' 슬롯에 연결
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

// ⭐️ 서버 메시지 처리 (바이너리 프로토콜 파싱)
void MainWindow::handleServerMessage()
{
    m_buffer.append(m_socket->readAll());

    while (true) {
        QDataStream in(&m_buffer, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_15);

        if (m_buffer.size() < sizeof(qint64)) break;
        qint64 totalSize;
        in >> totalSize;

        if (m_buffer.size() < totalSize) break;

        qint64 dataType;
        in >> dataType;

        if (dataType == Protocol::Chat_Message) {
            QString roomId;
            QString formattedMessage;
            in >> roomId >> formattedMessage;

            // ⭐️ 자신과 같은 회사(RoomId)의 메시지만 표시
            if (roomId == m_companyName) {
                m_chatTab->appendMessage(formattedMessage);
            }
        }
        m_buffer.remove(0, totalSize);
    }
}

// ⭐️ 채팅 탭의 요청을 받아 실제 메시지를 전송
void MainWindow::sendChatMessage(const QString& message)
{
    // ⭐️ 메시지를 보낼 때 자신의 RoomId와 ClientId를 사용
    writeData(Protocol::Chat_Message, m_companyName, m_myUserId, message);
}

// ⭐️ QDataStream을 이용해 서버에 데이터를 전송하는 함수 (관리자와 동일)
void MainWindow::writeData(qint64 dataType, const QString& roomId, qint64 clientId, const QString& message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << qint64(0);
    out << dataType;
    out << roomId;
    out << clientId;
    out << message;

    out.device()->seek(0);
    out << (qint64)block.size();

    m_socket->write(block);
    m_socket->flush();
}
