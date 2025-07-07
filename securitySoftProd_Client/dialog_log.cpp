#include "dialog_log.h"
#include "ui_dialog_log.h"
#include "dialog_signup.h"
#include "mainwindow.h"
#include "mainwindow_admin.h"
#include <QMessageBox>
#include <QDebug>
#include <QDataStream> // QDataStream 헤더 추가
#include <QJsonDocument>
#include <QtCore/qjsonobject.h>

// 서버와 동일한 프로토콜 정의 사용
namespace Protocol {
enum DataType : qint64 {
    Login_Request  = 0x07,
    //Login_Success  = 0x11, //데이터가 NULL이 아니면 Success
    //Login_Failure  = 0x12,//데이터가 오면 파싱해서 NULL이면 fail
};
}

Dialog_log* Dialog_log::instance = nullptr;

Dialog_log::Dialog_log(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_log)
    , socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    // pushButton_login의 clicked 시그널은 UI 에디터에서 on_pushButton_login_clicked로 자동 연결됩니다.
    // connect(ui->pushButton_login, &QPushButton::clicked, this, &Dialog_log::on_pushButton_login_clicked);
    connect(ui->pushButton_signUp, &QPushButton::clicked, this, &Dialog_log::on_pushButton_signUp_clicked);
    connect(socket, &QTcpSocket::readyRead, this, &Dialog_log::onReadyRead);

    connect(socket, &QTcpSocket::connected, this, [](){
        qDebug() << "[Client] Connected to server.";
    });
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError){
        QMessageBox::critical(this, "Connection Error", socket->errorString());
    });

    socket->connectToHost("127.0.0.1", 50000);
}

Dialog_log::~Dialog_log()
{
    delete ui;
    instance = nullptr;
}

Dialog_log* Dialog_log::getInstance(QWidget *parent)
{
    if (!instance) {
        instance = new Dialog_log(parent);
    }
    return instance;
}

// 로그인 버튼 클릭 시 바이너리 데이터 전송
void Dialog_log::on_pushButton_login_clicked()
{
    QString id = ui->lineEdit_id->text().trimmed();
    QString pw = ui->lineEdit_pw->text().trimmed();

    if (id.isEmpty() || pw.isEmpty()) {
        QMessageBox::warning(this, "Login", "Please enter both ID and password.");
        return;
    }
    // [전체크기]는 나중에 채우기 위해 0으로 남겨둠

    // [데이터타입][ID][PW] 순서로 데이터 쓰기
    //out << qint64(Protocol::Login_Request);
    QJsonObject Container;
    Container["id"] = id;
    Container["pw"] = pw;

    QJsonDocument jsonDoc(Container);
    QByteArray block;
    QByteArray Contain;
    block = jsonDoc.toJson();

    QDataStream out(&Contain, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << qint64(0) << qint64(0) << qint64(0) << "login info";
    Contain.append(block);
    qint64 totalSize;
    totalSize += Contain.size();
    // 맨 앞으로 가서 실제 블록 크기(=전체크기)를 씀
    out.device()->seek(0);
    out << qint64(Protocol::Login_Request) <<totalSize << totalSize;

    socket->write(Contain);
    socket->flush();
}

void Dialog_log::on_pushButton_signUp_clicked()
{
    Dialog_SignUp signUpDialog(this);
    signUpDialog.exec();
}

// 서버로부터 온 바이너리 응답 처리
void Dialog_log::onReadyRead()
{
    m_buffer.append(socket->readAll());

    while(true) {
        QDataStream in(&m_buffer, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_15);

        // 1. 전체 데이터 크기를 읽을 만큼 데이터가 있는지 확인
        if (m_buffer.size() < sizeof(qint64)) {
            break;
        }
        qint64 totalSize;
        in >> totalSize;

        // 2. 전체 메시지가 도착했는지 확인
        if (m_buffer.size() < totalSize) {
            break;
        }

        // --- 전체 메시지를 수신했으므로 파싱 시작 ---
        qint64 dataType;
        in >> dataType; // 실제 데이터 타입 읽기

        // if (dataType == Protocol::Login_Success) {
        //     qint64 clientId;
        //     QString roomId;
        //     in >> clientId >> roomId;

        //     qDebug() << "[Client] Login Success. ClientID:" << clientId << "RoomID:" << roomId;

        //     // MainWindow가 소켓 제어권을 갖도록 시그널 연결 해제
        //     disconnect(socket, &QTcpSocket::readyRead, this, &Dialog_log::onReadyRead);

        //     // 관리자 ID는 1000번대, 일반 클라이언트는 그 이하로 가정
        //     if (clientId >= 1000) {
        //         auto adminWin = new MainWindow_Admin(socket, clientId, nullptr);
        //         adminWin->setAttribute(Qt::WA_DeleteOnClose);
        //         adminWin->show();
        //     } else {
        //         auto clientWin = new MainWindow(socket, roomId, clientId, nullptr);
        //         clientWin->setAttribute(Qt::WA_DeleteOnClose);
        //         clientWin->show();
        //     }
        //     this->accept(); // 로그인 성공 시 현재 창 닫기

        // } else if (dataType == Protocol::Login_Failure) {
        //     qDebug() << "[Client] Login Failed.";
        //     QMessageBox::warning(this, "Login Failed", "Invalid ID or password.");
        // }

        // 처리한 메시지는 버퍼에서 제거
        m_buffer.remove(0, totalSize);
    }
}
