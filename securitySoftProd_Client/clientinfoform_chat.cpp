#include "clientinfoform_chat.h"
#include "ui_clientinfoform_chat.h"
#include <QTabWidget>
#include <QIcon>

ClientInfoForm_Chat::ClientInfoForm_Chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientInfoForm_Chat)
{
    ui->setupUi(this);
    // UI의 pushButton_client가 클릭되면 on_pushButton_client_clicked 슬롯이 호출됨
    connect(ui->pushButton_client, &QPushButton::clicked, this, &ClientInfoForm_Chat::on_pushButton_client_clicked);
}

ClientInfoForm_Chat::~ClientInfoForm_Chat()
{
    delete ui;
}

// '전송' 버튼이 눌리면 MainWindow에 시그널을 보냄
void ClientInfoForm_Chat::on_pushButton_client_clicked()
{
    QString messageText = ui->lineEdit->text().trimmed();
    if (messageText.isEmpty()) return;

    // 소켓에 직접 쓰는 대신 시그널 발생
    emit messageSendRequested(messageText);

    ui->lineEdit->clear();
}

// MainWindow에서 포맷된 메시지를 받아 표시하는 함수
void ClientInfoForm_Chat::appendMessage(const QString& formattedMessage)
{
    if (!formattedMessage.trimmed().isEmpty()) {
        ui->chatDisplay->append(formattedMessage);
        // 현재 활성화된 탭이 아니면 알림 아이콘 표시
        if (auto p = parentWidget(); p) {
            if (auto tab = qobject_cast<QTabWidget*>(p); tab && tab->currentWidget() != this) {
                showChatNotification();
            }
        }
    }
}

// --- 아래 함수들은 UI 알림 관련이므로 변경 없음 ---
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
