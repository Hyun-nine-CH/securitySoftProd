#include "clientinfoform_chat.h"
#include "ui_clientinfoform_chat.h"
#include "communication.h"
#include <QTabWidget>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollBar>
#include "Protocol.h"

ClientInfoForm_Chat::ClientInfoForm_Chat(QWidget *parent)
    : QWidget(parent), ui(new Ui::ClientInfoForm_Chat)
{
    ui->setupUi(this);

    // 메시지 전송 버튼 연결
    connect(ui->pushButton_client, &QPushButton::clicked, this, &ClientInfoForm_Chat::on_pushButton_client_clicked);

    // 파일 전송 버튼 연결
    connect(ui->pushButton_fileClient, &QPushButton::clicked, this, &ClientInfoForm_Chat::on_pushButton_fileClient_clicked);

    // 서버에서 메시지 응답 받기
    connect(Communication::getInstance(),&Communication::ReceiveChat,this,&ClientInfoForm_Chat::appendMessage);

    // 엔터키 이벤트 필터 설치
    ui->lineEdit->installEventFilter(this);

    // 채팅 기록 요청
    //requestChatHistory();
}

ClientInfoForm_Chat::~ClientInfoForm_Chat()
{
    delete ui;
}

// 엔터키 이벤트 처리
bool ClientInfoForm_Chat::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->lineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            on_pushButton_client_clicked();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

// '전송' 버튼 클릭 처리
void ClientInfoForm_Chat::on_pushButton_client_clicked()
{
    QString messageText = ui->lineEdit->text().trimmed();
    if (messageText.isEmpty()) return;
    Communication::getInstance()->SendChatMesg(messageText);

    ui->lineEdit->clear();
}

// 파일 전송 버튼 클릭 처리
void ClientInfoForm_Chat::on_pushButton_fileClient_clicked()
{
    // 파일 전송 기능 구현 (향후 확장)
}

// 채팅 기록 요청
void ClientInfoForm_Chat::requestChatHistory()
{
    QJsonObject requestObj;
    requestObj["roomId"] = Communication::getInstance()->getUserInfo()["RoomId"].toString();
    sendDataToServer(Protocol::Chat_History_Request, requestObj, "chat_history");
}

// 서버에 데이터 전송
void ClientInfoForm_Chat::sendDataToServer(qint64 dataType, const QJsonObject& payload, const QString& filename)
{

}

// 서버로부터 받은 데이터 처리
void ClientInfoForm_Chat::handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename)
{

}

// 메시지 표시 함수
void ClientInfoForm_Chat::appendMessage(const QBuffer &buffer)
{
    QByteArray arr = buffer.data();
    arr.remove(0, buffer.pos());
    QJsonObject Mesg = QJsonDocument::fromJson(arr).object();
    QString id, m;
    id = Mesg["nickname"].toString();
    m = Mesg["message"].toString();

    QString formattedMessage;
    formattedMessage = id + " : " + m;
    ui->chatDisplay->append(formattedMessage);
    // 스크롤을 최하단으로 이동
    QScrollBar *scrollBar = ui->chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());

}

// 채팅 알림 아이콘 표시
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

// 채팅 알림 아이콘 제거
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

// 탭 활성화 시 호출될 함수
void ClientInfoForm_Chat::onChatTabActivated()
{
    clearChatNotification();

    // 탭 활성화 시 스크롤을 최하단으로 이동
    QScrollBar *scrollBar = ui->chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
