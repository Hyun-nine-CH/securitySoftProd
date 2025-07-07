#include "dialog_log.h"
#include "ui_dialog_log.h"
#include "dialog_signup.h"
#include "mainwindow.h"
#include "mainwindow_admin.h"
#include <QMessageBox>
#include <QDebug>

Dialog_log* Dialog_log::instance = nullptr;

Dialog_log::Dialog_log(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_log)
    , socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(ui->pushButton_login, &QPushButton::clicked, this, &Dialog_log::onPushButton_loginClicked);
    connect(ui->pushButton_signUp, &QPushButton::clicked, this, &Dialog_log::onPushButton_signUpClicked);
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

void Dialog_log::onPushButton_loginClicked()
{
    QString id = ui->lineEdit_id->text().trimmed();
    QString pw = ui->lineEdit_pw->text().trimmed();

    if (id.isEmpty() || pw.isEmpty()) {
        QMessageBox::warning(this, "Login", "Please enter both ID and password.");
        return;
    }

    QString loginMsg = QString("/login %1 %2\n").arg(id, pw);
    socket->write(loginMsg.toUtf8());
}

void Dialog_log::onPushButton_signUpClicked()
{
    Dialog_SignUp signUpDialog(this);
    signUpDialog.exec();
}

void Dialog_log::onReadyRead()
{
    if (!socket->canReadLine()) return;

    const QString response = QString::fromUtf8(socket->readLine()).trimmed();
    qDebug() << "[Client] Server response:" << response;

    if (response == "LOGIN_SUCCESS:ADMIN") {
        // ⭐️ 로그인 후에는 소켓의 readyRead 시그널 연결을 끊어야 MainWindow가 제어권을 가짐
        disconnect(socket, &QTcpSocket::readyRead, this, &Dialog_log::onReadyRead);
        auto adminWin = new MainWindow_Admin(socket);
        adminWin->setAttribute(Qt::WA_DeleteOnClose); // 창 닫으면 메모리 해제
        adminWin->show();
        this->accept();
    } else if (response.startsWith("LOGIN_SUCCESS:CLIENT")) {
        disconnect(socket, &QTcpSocket::readyRead, this, &Dialog_log::onReadyRead);
        QString companyName = response.section(':', 2); // "A회사" 추출
        auto clientWin = new MainWindow(socket, companyName); // 생성자에 companyName 전달
        clientWin->setAttribute(Qt::WA_DeleteOnClose);
        clientWin->show();
        this->accept();
    } else if (response == "LOGIN_FAIL") {
        QMessageBox::warning(this, "Login Failed", "Invalid ID or password.");
    } else if (!response.isEmpty()){
        qDebug() << "[Client] Unknown server response:" << response;
    }
}
