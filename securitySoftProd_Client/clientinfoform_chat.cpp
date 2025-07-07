#include "clientinfoform_chat.h"
#include "ui_clientinfoform_chat.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTabWidget>
#include <QIcon>
#include <QMessageBox>

// 생성자 구현 변경
ClientInfoForm_Chat::ClientInfoForm_Chat(QTcpSocket *socket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientInfoForm_Chat)
    , m_socket(socket)
{
    ui->setupUi(this);

    // 메시지 수신(readyRead) connect는 MainWindow에서 하므로 여기서는 삭제!
    connect(ui->pushButton_client, &QPushButton::clicked, this, &ClientInfoForm_Chat::sendMessage);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &ClientInfoForm_Chat::handleSocketError);
}

ClientInfoForm_Chat::~ClientInfoForm_Chat()
{
    delete ui;
}

// ⭐️ sendMessage 함수를 이전 버전으로 복원
void ClientInfoForm_Chat::sendMessage()
{
    QString messageText = ui->lineEdit->text().trimmed();
    if (messageText.isEmpty() || !m_socket) return;

    QJsonObject messageObject;
    messageObject["type"] = "chat";
    // 클라이언트는 target_company를 보낼 필요 없음 (서버가 소켓으로 식별)
    messageObject["message"] = messageText;

    QByteArray data = QJsonDocument(messageObject).toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(data);

    ui->lineEdit->clear();
    // ⭐️ "[나]"를 로컬에 추가하는 코드 복원!
    ui->chatDisplay->append("[나] " + messageText);
}

// MainWindow에서 호출하는 메시지 표시 함수
void ClientInfoForm_Chat::appendMessage(const QString& message)
{
    if (!message.trimmed().isEmpty()) {
        ui->chatDisplay->append(message);
        // 현재 활성화된 탭이 아니면 알림 표시
        if (auto p = parentWidget(); p) {
            if (auto tab = qobject_cast<QTabWidget*>(p); tab && tab->currentWidget() != this) {
                showChatNotification();
            }
        }
    }
}

void ClientInfoForm_Chat::handleSocketError(QAbstractSocket::SocketError socketError)
{
    // Q_UNUSED(socketError);
    QMessageBox::critical(this, "Socket Error", m_socket->errorString());
}

void ClientInfoForm_Chat::showChatNotification()
{
    if (auto p = parentWidget(); p) {
        if (auto tabWidget = qobject_cast<QTabWidget*>(p)) {
            int tabIndex = tabWidget->indexOf(this);
            if (tabIndex >= 0) {
                tabWidget->setTabIcon(tabIndex, QIcon(":/images/alert.png"));
            }
        }
    }
}

void ClientInfoForm_Chat::clearChatNotification()
{
    if (auto p = parentWidget(); p) {
        if (auto tabWidget = qobject_cast<QTabWidget*>(p)) {
            int tabIndex = tabWidget->indexOf(this);
            if (tabIndex >= 0) {
                tabWidget->setTabIcon(tabIndex, QIcon()); // 아이콘 제거
            }
        }
    }
}

void ClientInfoForm_Chat::onChatTabActivated()
{
    clearChatNotification();
}
