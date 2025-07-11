#include "dialog_log.h"
#include "ui_dialog_log.h"
#include "dialog_signup.h"
#include <QMessageBox>
#include <QDebug>
#include <QDataStream>
#include <QJsonDocument>
#include <QKeyEvent>

// 서버와 통신할 프로토콜 정의
namespace Protocol {
enum DataType : qint64 {
    Login_Request = 0x07,
};
}

Dialog_log* Dialog_log::instance = nullptr;

Dialog_log::Dialog_log(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog_log), socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    // --- UI/UX 개선 기능 추가 ---
    // 1. 비밀번호 입력란 마스킹 처리
    ui->lineEdit_pw->setEchoMode(QLineEdit::Password);
    // 2. 비밀번호 입력란에 이벤트 필터 설치 (Enter 키 감지용)
    ui->lineEdit_pw->installEventFilter(this);

    connect(ui->pushButton_login, &QPushButton::clicked, this, &Dialog_log::on_pushButton_login_clicked);
    connect(ui->pushButton_signUp, &QPushButton::clicked, this, &Dialog_log::on_pushButton_signUp_clicked);
    connect(socket, &QTcpSocket::readyRead, this, &Dialog_log::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, [](){ qDebug() << "[Client] Connected to server."; });
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError){
        QMessageBox::critical(this, "Connection Error", socket->errorString());
    });
    socket->connectToHost("127.0.0.1", 50000);
}

Dialog_log::~Dialog_log() {
    delete ui;
    instance = nullptr;
}

Dialog_log* Dialog_log::getInstance(QWidget *parent) {
    if (!instance) {
        instance = new Dialog_log(parent);
    }
    return instance;
}

// Enter 키를 누르면 로그인 버튼이 클릭되도록 하는 이벤트 필터
bool Dialog_log::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->lineEdit_pw && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            on_pushButton_login_clicked();
            return true; // 이벤트 처리 완료
        }
    }
    return QDialog::eventFilter(watched, event);
}

void Dialog_log::on_pushButton_login_clicked() {
    QString id = ui->lineEdit_id->text().trimmed();
    QString pw = ui->lineEdit_pw->text().trimmed();
    if (id.isEmpty() || pw.isEmpty()) {
        QMessageBox::warning(this, "Login", "Please enter both ID and password.");
        return;
    }
    QJsonObject loginObject;
    loginObject["id"] = id;
    loginObject["pw"] = pw;
    QByteArray payload = QJsonDocument(loginObject).toJson();
    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << qint64(0) << qint64(0) << qint64(0) << "login info";
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Login_Request) << totalSize << totalSize;
    socket->write(blockToSend);
    socket->flush();
}

void Dialog_log::on_pushButton_signUp_clicked() {
    Dialog_SignUp signUpDialog(this);
    signUpDialog.exec();
}

// Dialog_log::on_pushButton_logout_clicked() 같은 슬롯에서
void Dialog_log::on_pushButton_logout_clicked() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost(); // 서버와의 연결 종료
        // socket->waitForDisconnected(); // 필요하다면 연결이 완전히 끊어질 때까지 기다림
    }
    // 필요한 경우 UI 상태 초기화 또는 다른 다이얼로그 표시
}

void Dialog_log::onReadyRead() {
    m_buffer.append(socket->readAll());
    while(true) {
        QDataStream in(&m_buffer, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_15);
        if (m_buffer.size() < (3 * sizeof(qint64))) break;
        qint64 dataType, totalSize, currentPacketSize;
        in >> dataType >> totalSize >> currentPacketSize;
        if (m_buffer.size() < totalSize) break;
        QString filename;
        in >> filename;
        QByteArray payload = m_buffer.mid(in.device()->pos(), totalSize - in.device()->pos());
        QJsonDocument doc = QJsonDocument::fromJson(payload);

        if (doc.isObject() && !doc.object().isEmpty()) {
            m_userInfo = doc.object();
            this->accept();
        } else {
            QMessageBox::warning(this, "로그인 실패", "아이디 또는 비밀번호가 일치하지 않습니다.");
        }
        m_buffer.remove(0, totalSize);
    }
}

QJsonObject Dialog_log::getUserInfo() const { return m_userInfo; }
QTcpSocket* Dialog_log::getSocket() { return socket; }
