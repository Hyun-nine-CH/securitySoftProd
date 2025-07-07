#include "clientinfoform_chat.h"
#include "ui_clientinfoform_chat.h"
#include <QTabWidget>
#include <QIcon>
#include <QMessageBox>

// ⭐️ 생성자에서 소켓 포인터를 받지 않음
ClientInfoForm_Chat::ClientInfoForm_Chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientInfoForm_Chat)
{
    ui->setupUi(this);
    connect(ui->pushButton_client, &QPushButton::clicked, this, &ClientInfoForm_Chat::on_sendButton_clicked);
}

ClientInfoForm_Chat::~ClientInfoForm_Chat()
{
    delete ui;
}

// ⭐️ '전송' 버튼이 눌리면 MainWindow에 시그널을 보냄
void ClientInfoForm_Chat::on_sendButton_clicked()
{
    QString messageText = ui->lineEdit->text().trimmed();
    if (messageText.isEmpty()) return;

    // ⭐️ 소켓에 직접 쓰는 대신 시그널 발생
    emit messageSendRequested(messageText);

    ui->lineEdit->clear();
    // ⭐️ "[나]" 메시지는 로컬에 바로 표시
    ui->chatDisplay->append("[나] " + messageText);
}

// ⭐️ MainWindow에서 포맷된 메시지를 받아 표시하는 함수
void ClientInfoForm_Chat::appendMessage(const QString& formattedMessage)
{
    if (!formattedMessage.trimmed().isEmpty()) {
        ui->chatDisplay->append(formattedMessage);
        if (auto p = parentWidget(); p) {
            if (auto tab = qobject_cast<QTabWidget*>(p); tab && tab->currentWidget() != this) {
                showChatNotification();
            }
        }
    }
}

// ⭐️ 아래 함수들은 UI 관련이므로 그대로 유지
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
