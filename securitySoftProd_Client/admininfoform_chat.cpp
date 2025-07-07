#include "admininfoform_chat.h"
#include "ui_admininfoform_chat.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTabWidget>
#include <QIcon>
#include <QMessageBox>

// 생성자 구현 변경
AdminInfoForm_Chat::AdminInfoForm_Chat(QTcpSocket *socket, const QString& companyName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminInfoForm_Chat)
    , m_socket(socket)
    , m_companyName(companyName) // 전달받은 회사 이름 저장
{
    ui->setupUi(this);

    // 메시지 수신(readyRead) connect는 MainWindow에서 하므로 여기서는 삭제!
    connect(ui->pushButton_admin, &QPushButton::clicked, this, &AdminInfoForm_Chat::sendMessage);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &AdminInfoForm_Chat::handleSocketError);
}

AdminInfoForm_Chat::~AdminInfoForm_Chat()
{
    delete ui;
}

// ⭐️ sendMessage 함수를 이전 버전으로 복원
void AdminInfoForm_Chat::sendMessage()
{
    QString messageText = ui->lineEdit->text().trimmed();
    if (messageText.isEmpty() || !m_socket) return;

    QJsonObject messageObject;
    messageObject["type"] = "chat";
    messageObject["target_company"] = m_companyName; // 이 채팅방의 회사를 타겟으로 지정
    messageObject["message"] = messageText;

    QByteArray data = QJsonDocument(messageObject).toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(data);

    ui->lineEdit->clear();
    // ⭐️ "[나]"를 로컬에 추가하는 코드 복원!
    ui->chatDisplay->append("[나] " + messageText);
}

// MainWindow에서 호출하는 메시지 표시 함수
void AdminInfoForm_Chat::appendMessage(const QString& message)
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

void AdminInfoForm_Chat::handleSocketError(QAbstractSocket::SocketError socketError)
{
    // Q_UNUSED(socketError);
    QMessageBox::critical(this, "Socket Error", m_socket->errorString());
}

void AdminInfoForm_Chat::showChatNotification()
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

void AdminInfoForm_Chat::clearChatNotification()
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

void AdminInfoForm_Chat::onChatTabActivated()
{
    clearChatNotification();
}
