#include "admininfoform_chat.h"
#include "ui_admininfoform_chat.h"
#include <QTabWidget>
#include <QIcon>
#include <QMessageBox>

// ⭐️ 생성자 구현 변경
AdminInfoForm_Chat::AdminInfoForm_Chat(const QString& companyName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminInfoForm_Chat)
    , m_companyName(companyName)
{
    ui->setupUi(this);
    // ⭐️ 위젯 내부의 버튼 클릭 시 on_sendButton_clicked 슬롯 호출
    connect(ui->pushButton_admin, &QPushButton::clicked, this, &AdminInfoForm_Chat::on_sendButton_clicked);
}

AdminInfoForm_Chat::~AdminInfoForm_Chat()
{
    delete ui;
}

// ⭐️ '전송' 버튼이 눌리면 MainWindow_Admin에 시그널을 보냄
void AdminInfoForm_Chat::on_sendButton_clicked()
{
    QString messageText = ui->lineEdit->text().trimmed();
    if (messageText.isEmpty()) return;

    // ⭐️ 소켓에 직접 쓰는 대신 시그널 발생
    emit messageSendRequested(m_companyName, messageText);

    ui->lineEdit->clear();
    // ⭐️ "[나]" 메시지는 로컬에 바로 표시
    ui->chatDisplay->append("[나] " + messageText);
}

// ⭐️ MainWindow에서 포맷된 메시지를 받아 표시하는 함수
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

// ⭐️ 아래 함수들은 UI 관련이므로 그대로 유지
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
