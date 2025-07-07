#include "mainwindow_admin.h"
#include "ui_mainwindow_admin.h"
#include "admininfoform_prod.h"
#include "admininfoform.h"
#include "admininfoform_chat.h"
#include <QDataStream>

// ⭐️ 데이터 타입 정의 (프로토콜)
namespace Protocol {
enum DataType : qint64 {
    Chat_Message = 0x01,
    // 다른 데이터 타입 추가 가능
};
}

MainWindow_Admin::MainWindow_Admin(QTcpSocket* socket, qint64 adminId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow_Admin)
    , m_socket(socket)
    , m_myUserId(adminId) // ⭐️ 관리자 ID 저장
{
    ui->setupUi(this);

    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow_Admin::handleServerMessage);

    // 고객/제품 관리 탭 생성은 동일
    AdminInfoForm* clientTab = new AdminInfoForm(this);
    ui->tabWidget->addTab(clientTab, tr("고객 관리"));
    AdminInfoForm_Prod* prodTab = new AdminInfoForm_Prod(this);
    ui->tabWidget->addTab(prodTab, tr("제품 관리"));

    // 탭 전환 시 알림 제거 기능
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

// ⭐️ 서버 메시지 처리 (바이너리 프로토콜 파싱)
void MainWindow_Admin::handleServerMessage()
{
    m_buffer.append(m_socket->readAll());

    while (true) {
        QDataStream in(&m_buffer, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_15);

        // 1. 전체 데이터 크기를 읽을 만큼 데이터가 있는지 확인
        if (m_buffer.size() < sizeof(qint64)) break;
        qint64 totalSize;
        in >> totalSize;

        // 2. 전체 메시지가 도착했는지 확인
        if (m_buffer.size() < totalSize) break;

        // ⭐️ 메시지 하나를 완전히 수신했으므로 파싱 시작
        qint64 dataType;
        in >> dataType;

        if (dataType == Protocol::Chat_Message) {
            QString roomId;
            QString formattedMessage;
            in >> roomId >> formattedMessage;

            if (roomId.isEmpty()) continue;

            createOrSwitchToChatTab(roomId); // 채팅 탭 생성 또는 전환
            if(m_chatTabs.contains(roomId)) {
                m_chatTabs.value(roomId)->appendMessage(formattedMessage);
            }
        }
        // ⭐️ 처리한 메시지는 버퍼에서 제거
        m_buffer.remove(0, totalSize);
    }
}

// ⭐️ 채팅 탭의 요청을 받아 실제 메시지를 전송
void MainWindow_Admin::sendChatMessage(const QString& companyName, const QString& message)
{
    writeData(Protocol::Chat_Message, companyName, m_myUserId, message);
}

// ⭐️ 새 채팅 탭을 만들거나 기존 탭으로 이동
void MainWindow_Admin::createOrSwitchToChatTab(const QString& companyName)
{
    if (m_chatTabs.contains(companyName)) {
        ui->tabWidget->setCurrentWidget(m_chatTabs.value(companyName));
        return;
    }

    AdminInfoForm_Chat* chatTab = new AdminInfoForm_Chat(companyName, this);
    // ⭐️ 채팅 탭의 '전송 요청' 시그널을 MainWindow의 '전송' 슬롯에 연결
    connect(chatTab, &AdminInfoForm_Chat::messageSendRequested, this, &MainWindow_Admin::sendChatMessage);

    m_chatTabs.insert(companyName, chatTab);
    int newIndex = ui->tabWidget->addTab(chatTab, QString("%1 채팅").arg(companyName));
    ui->tabWidget->setCurrentIndex(newIndex);
}


// ⭐️ QDataStream을 이용해 서버에 데이터를 전송하는 함수
void MainWindow_Admin::writeData(qint64 dataType, const QString& roomId, qint64 clientId, const QString& message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    // ⭐️ [전체크기]는 나중에 채우기 위해 0으로 남겨둠
    out << qint64(0);
    // ⭐️ [데이터타입][RoomID][ClientID][메시지] 순서로 데이터 쓰기
    out << dataType;
    out << roomId;
    out << clientId;
    out << message;

    // ⭐️ 맨 앞으로 가서 실제 블록 크기(=전체크기)를 씀
    out.device()->seek(0);
    out << (qint64)block.size();

    m_socket->write(block);
    m_socket->flush();
}
