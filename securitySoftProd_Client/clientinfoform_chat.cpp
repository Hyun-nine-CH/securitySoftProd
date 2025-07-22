#include "clientinfoform_chat.h"
#include "ui_clientinfoform_chat.h"
#include "communication.h"
#include <QTabWidget>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollBar>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

ClientInfoForm_Chat::ClientInfoForm_Chat(QWidget *parent)
    : QWidget(parent), ui(new Ui::ClientInfoForm_Chat)
{
    ui->setupUi(this);

    // 메시지 전송 버튼 연결
    connect(ui->pushButton_client, &QPushButton::clicked, this, &ClientInfoForm_Chat::on_pushButton_client_clicked);
    // 파일 전송 버튼 연결
    connect(ui->pushButton_exit, &QPushButton::clicked,this, &ClientInfoForm_Chat::ExitCorpRoom);
    // 서버에서 메시지 응답 받기
    connect(Communication::getInstance(),&Communication::ReceiveChat,this,&ClientInfoForm_Chat::appendMessage);
    // 초대 메시지 받기
    connect(Communication::getInstance(),&Communication::InviteFromCorp,this,&ClientInfoForm_Chat::AlertInvite);
    // 채팅방 나가기
    connect(this, &ClientInfoForm_Chat::exits,Communication::getInstance(),&Communication::RequestInviteExit);

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
            //on_pushButton_client_clicked();
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
    if(isInvite){
        QJsonObject ChatObject;
        ChatObject["message"] = messageText;
        ChatObject["nickname"] = Communication::getInstance()->getUserInfo().value("id").toString();
        ChatObject["RoomId"] = "Corp";
        QByteArray payload = QJsonDocument(ChatObject).toJson();
        Communication::getInstance()->SendChatMesg(payload);
    }else{
        Communication::getInstance()->SendChatMesg(messageText);
    }
    ui->lineEdit->clear();

    // 메시지 입력 후 스크롤을 최하단으로 이동
    QScrollBar *scrollBar = ui->chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

// // 파일 전송 버튼 클릭 처리
void ClientInfoForm_Chat::on_pushButton_fileClient_clicked()
{
    // QFileDialog를 사용하여 사용자에게 파일을 선택하도록 합니다.
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("전송할 PNG 파일 선택"),
                                                    QString(), // 초기 디렉터리 (비워두면 마지막 사용 경로)
                                                    tr("PNG 파일 (*.png)"));

    if (!filePath.isEmpty()) { // 사용자가 파일을 선택한 경우
        QFile* fileToTransfer = new QFile(filePath); // 선택된 경로로 QFile 객체 동적 생성

        // 파일을 읽기 모드로 열기 시도
        if (fileToTransfer->open(QFile::ReadOnly)) {
            qDebug() << "File selected and successfully opened by ClientInfoForm_Chat: " << filePath;
            //->startFileTransfer(fileToTransfer);
            Communication::getInstance()->SendFile(fileToTransfer);
        } else {
            // 파일 열기 실패 시
            qDebug() << "Failed to open file: " << filePath << ". Error: " << fileToTransfer->errorString();
            delete fileToTransfer; // 열기 실패했으므로 즉시 해제
            fileToTransfer = nullptr;
            QMessageBox::warning(this, tr("파일 열기 오류"), tr("선택한 파일을 열 수 없습니다:\n%1").arg(filePath));
        }
    } else {
        qDebug() << "File selection cancelled by user.";
    }
}

void ClientInfoForm_Chat::AlertInvite()
{
    QMessageBox::information(this, "강제 초대", "관리자와의 채팅으로 전환됩니다");
    isInvite = true;
}

void ClientInfoForm_Chat::ExitCorpRoom()
{
    if(isInvite){
        QMessageBox::information(this, "나가기", "관리자와의 채팅에서 빠져나갑니다");
        isInvite = false;
        ui->chatDisplay->clear();
        emit exits();
    }
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

// // 채팅 알림 아이콘 표시
// void ClientInfoForm_Chat::showChatNotification()
// {
//     if (auto p = parentWidget(); p) {
//         if (auto tabWidget = qobject_cast<QTabWidget*>(p)) {
//             int tabIndex = tabWidget->indexOf(this);
//             if (tabIndex >= 0) {
//                 tabWidget->setTabIcon(tabIndex, QIcon(":/images/alert.png"));
//             }
//         }
//     }
// }

// // 채팅 알림 아이콘 제거
// void ClientInfoForm_Chat::clearChatNotification()
// {
//     if (auto p = parentWidget(); p) {
//         if (auto tabWidget = qobject_cast<QTabWidget*>(p)) {
//             int tabIndex = tabWidget->indexOf(this);
//             if (tabIndex >= 0) {
//                 tabWidget->setTabIcon(tabIndex, QIcon()); // 아이콘 제거
//             }
//         }
//     }
// }

// // 탭 활성화 시 호출될 함수
// void ClientInfoForm_Chat::onChatTabActivated()
// {
//     clearChatNotification();

//     // 탭 활성화 시 스크롤을 최하단으로 이동
//     QScrollBar *scrollBar = ui->chatDisplay->verticalScrollBar();
//     scrollBar->setValue(scrollBar->maximum());
// }
