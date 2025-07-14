// #include "clientinfoform_chat.h"
// #include "ui_clientinfoform_chat.h"
// #include <QTabWidget>
// #include <QIcon>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QScrollBar>
// #include "Protocol.h"

// ClientInfoForm_Chat::ClientInfoForm_Chat(QTcpSocket* socket, const QJsonObject& userInfo, QWidget *parent)
//     : QWidget(parent)
//     , ui(new Ui::ClientInfoForm_Chat)
//     , m_socket(socket)
//     , m_userInfo(userInfo)
// {
//     ui->setupUi(this);

//     // 메시지 전송 버튼 연결
//     connect(ui->pushButton_client, &QPushButton::clicked, this, &ClientInfoForm_Chat::on_pushButton_client_clicked);

//     // 파일 전송 버튼 연결
//     connect(ui->pushButton_fileClient, &QPushButton::clicked, this, &ClientInfoForm_Chat::on_pushButton_fileClient_clicked);

//     // 엔터키 이벤트 필터 설치
//     ui->lineEdit->installEventFilter(this);

//     // 채팅 기록 요청
//     requestChatHistory();
// }

// ClientInfoForm_Chat::~ClientInfoForm_Chat()
// {
//     delete ui;
// }

// // 엔터키 이벤트 처리
// bool ClientInfoForm_Chat::eventFilter(QObject *watched, QEvent *event)
// {
//     if (watched == ui->lineEdit && event->type() == QEvent::KeyPress) {
//         QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
//         if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
//             on_pushButton_client_clicked();
//             return true;
//         }
//     }
//     return QWidget::eventFilter(watched, event);
// }

// // '전송' 버튼 클릭 처리
// void ClientInfoForm_Chat::on_pushButton_client_clicked()
// {
//     QString messageText = ui->lineEdit->text().trimmed();
//     if (messageText.isEmpty()) return;

//     // 닉네임 생성
//     QString nickName = QString("[%1][%2][%3]")
//                            .arg(m_userInfo["RoomId"].toString())
//                            .arg(m_userInfo["department"].toString())
//                            .arg(m_userInfo["manager"].toString());

//     // 메시지 객체 생성
//     QJsonObject chatObject;
//     chatObject["nickName"] = nickName;
//     chatObject["message"] = messageText;
//     chatObject["roomId"] = m_userInfo["RoomId"].toString();

//     // 내가 보낸 메시지는 로컬에 바로 표시
//     appendMessage(nickName + ": " + messageText);

//     // 서버로 전송
//     sendDataToServer(Protocol::Chat_Message, chatObject, m_userInfo["RoomId"].toString());

//     ui->lineEdit->clear();
// }

// // 파일 전송 버튼 클릭 처리
// void ClientInfoForm_Chat::on_pushButton_fileClient_clicked()
// {
//     // 파일 전송 기능 구현 (향후 확장)
// }

// // 채팅 기록 요청
// void ClientInfoForm_Chat::requestChatHistory()
// {
//     QJsonObject requestObj;
//     requestObj["roomId"] = m_userInfo["RoomId"].toString();
//     sendDataToServer(Protocol::Chat_History_Request, requestObj, "chat_history");
// }

// // 서버에 데이터 전송
// void ClientInfoForm_Chat::sendDataToServer(qint64 dataType, const QJsonObject& payload, const QString& filename)
// {
//     QByteArray blockToSend;
//     QDataStream out(&blockToSend, QIODevice::WriteOnly);
//     out.setVersion(QDataStream::Qt_5_15);
//     out << qint64(0) << qint64(0) << qint64(0) << filename;
//     blockToSend.append(QJsonDocument(payload).toJson());
//     qint64 totalSize = blockToSend.size();
//     out.device()->seek(0);
//     out << dataType << totalSize << totalSize;
//     m_socket->write(blockToSend);
//     m_socket->flush();
// }

// // 서버로부터 받은 데이터 처리
// void ClientInfoForm_Chat::handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename)
// {
//     QJsonDocument doc = QJsonDocument::fromJson(payload);

//     if (dataType == Protocol::Chat_Message) {
//         if (doc.isObject()) {
//             QJsonObject chatObj = doc.object();
//             QString roomId = chatObj["roomId"].toString();

//             // 내 RoomId와 메시지의 roomId가 일치하는지 확인
//             if (roomId == m_userInfo["RoomId"].toString()) {
//                 QString formattedMessage = chatObj["nickName"].toString() + ": " + chatObj["message"].toString();
//                 appendMessage(formattedMessage);
//             }
//         }
//     }
//     else if (dataType == Protocol::Chat_History_Response) {
//         if (doc.isObject()) {
//             QJsonObject historyObj = doc.object();
//             QString roomId = historyObj["roomId"].toString();

//             // 내 RoomId와 일치하는지 확인
//             if (roomId == m_userInfo["RoomId"].toString()) {
//                 QJsonArray messages = historyObj["messages"].toArray();
//                 ui->chatDisplay->clear(); // 기존 내용 지우고 기록 표시

//                 for (int i = 0; i < messages.size(); ++i) {
//                     QJsonObject msgObj = messages[i].toObject();
//                     QString formattedMessage = msgObj["nickName"].toString() + ": " + msgObj["message"].toString();
//                     appendMessage(formattedMessage);
//                 }
//                 appendMessage("--- 채팅 기록 로드 완료 ---");
//             }
//         }
//     }
// }

// // 메시지 표시 함수
// void ClientInfoForm_Chat::appendMessage(const QString& formattedMessage)
// {
//     if (!formattedMessage.trimmed().isEmpty()) {
//         ui->chatDisplay->append(formattedMessage);

//         // 스크롤을 최하단으로 이동
//         QScrollBar *scrollBar = ui->chatDisplay->verticalScrollBar();
//         scrollBar->setValue(scrollBar->maximum());
//     }
// }

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
