#include "admininfoform_chat.h"
#include "ui_admininfoform_chat.h"
#include <QTabWidget>
#include <QIcon>

AdminInfoForm_Chat::AdminInfoForm_Chat(const QString& companyName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminInfoForm_Chat)
    , m_companyName(companyName)
{
    ui->setupUi(this);
    connect(ui->pushButton_admin, &QPushButton::clicked, this, &AdminInfoForm_Chat::on_pushButton_admin_clicked);
}

AdminInfoForm_Chat::~AdminInfoForm_Chat()
{
    delete ui;
}

// '전송' 버튼이 눌리면 MainWindow_Admin에 시그널을 보냄
void AdminInfoForm_Chat::on_pushButton_admin_clicked()
{
    QString messageText = ui->lineEdit->text().trimmed();
    if (messageText.isEmpty()) return;

    // 어느 회사(m_companyName)에게 보내는지와 메시지 내용을 함께 시그널로 보냄
    emit messageSendRequested(m_companyName, messageText);

    ui->lineEdit->clear();
}

// MainWindow_Admin에서 포맷된 메시지를 받아 표시하는 함수
void AdminInfoForm_Chat::appendMessage(const QString& formattedMessage)
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

// --- 아래 함수들은 UI 알림 관련이므로 변경 없음 ---
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
