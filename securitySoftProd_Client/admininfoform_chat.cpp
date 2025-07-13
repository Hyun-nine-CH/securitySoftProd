#include "admininfoform_chat.h"
#include "ui_admininfoform_chat.h"
#include <QTabWidget>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollBar>
#include "Protocol.h"

AdminInfoForm_Chat::AdminInfoForm_Chat(const QString& companyName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminInfoForm_Chat)
    , m_companyName(companyName)
{
    ui->setupUi(this);
    // UI의 pushButton_admin이 클릭되면 on_pushButton_admin_clicked 슬롯이 호출됨
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

    // 메시지 입력 후 스크롤을 최하단으로 이동
    QScrollBar *scrollBar = ui->chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

// MainWindow_Admin에서 포맷된 메시지를 받아 표시하는 함수
void AdminInfoForm_Chat::appendMessage(const QString& formattedMessage)
{
    if (!formattedMessage.trimmed().isEmpty()) {
        ui->chatDisplay->append(formattedMessage);

        // 스크롤을 최하단으로 이동
        QScrollBar *scrollBar = ui->chatDisplay->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());

        // 현재 활성화된 탭이 아니면 알림 아이콘 표시
        if (auto p = parentWidget(); p) {
            if (auto tab = qobject_cast<QTabWidget*>(p); tab && tab->currentWidget() != this) {
                showChatNotification();
            }
        }
    }
}

// 서버에서 받은 데이터 처리 (MainWindow_Admin에서 호출)
void AdminInfoForm_Chat::handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename)
{
    QJsonDocument doc = QJsonDocument::fromJson(payload);

    if (dataType == Protocol::Chat_Message) {
        if (doc.isObject()) {
            QJsonObject chatObj = doc.object();
            QString roomId = chatObj["roomId"].toString();

            // 이 채팅방에 해당하는 메시지인지 확인
            if (roomId == m_companyName) {
                QString formattedMessage = chatObj["nickName"].toString() + ": " + chatObj["message"].toString();
                appendMessage(formattedMessage);
            }
        }
    } else if (dataType == Protocol::Chat_History_Response) {
        if (doc.isObject()) {
            QJsonObject historyObj = doc.object();
            QString roomId = historyObj["roomId"].toString();

            // 이 채팅방에 해당하는 기록인지 확인
            if (roomId == m_companyName) {
                QJsonArray messages = historyObj["messages"].toArray();

                // 채팅 기록 표시
                for (const QJsonValue &value : messages) {
                    QJsonObject msgObj = value.toObject();
                    QString formattedMessage = msgObj["nickName"].toString() + ": " + msgObj["message"].toString();
                    appendMessage(formattedMessage);
                }
                appendMessage("--- 채팅 기록 로드 완료 ---");
            }
        }
    }
}

// 채팅 알림 아이콘 표시
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

// 채팅 알림 아이콘 제거
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

// 탭 활성화 시 호출될 함수
void AdminInfoForm_Chat::onChatTabActivated()
{
    clearChatNotification();
}
