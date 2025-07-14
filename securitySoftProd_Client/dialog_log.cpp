#include "dialog_log.h"
#include "ui_dialog_log.h"
#include "dialog_signup.h"
#include <QMessageBox>
#include <QDebug>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QKeyEvent>

#include "communication.h"

Dialog_log::Dialog_log(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog_log)
{
    ui->setupUi(this);

    qDebug() << "Dialog_log 생성자 호출됨";

    // --- UI/UX 개선 기능 추가 ---
    // 1. 비밀번호 입력란 마스킹 처리
    ui->lineEdit_pw->setEchoMode(QLineEdit::Password);
    // 2. 비밀번호 입력란에 이벤트 필터 설치 (Enter 키 감지용)
    ui->lineEdit_pw->installEventFilter(this);

    qDebug() << "Dialog_log 초기화 완료";
}

Dialog_log::~Dialog_log() {
    qDebug() << "Dialog_log 소멸자 호출됨";
    delete ui;
}

void Dialog_log::on_pushButton_login_clicked() {
    QString id = ui->lineEdit_id->text().trimmed();
    QString pw = ui->lineEdit_pw->text().trimmed();

    Communication::getInstance()->SendLoginConfirm(id,pw);
    connect(Communication::getInstance(),&Communication::LoginSuccess,this,&Dialog_log::LoginPass);
    connect(Communication::getInstance(),&Communication::LoginFail,this,&Dialog_log::LoginFail);
}

void Dialog_log::LoginPass()
{
    this->accept();
    qDebug() << "로그인 성공";
}

void Dialog_log::LoginFail()
{
    qDebug() << "로그인 실패";
    QMessageBox::warning(this, "회원가입 실패", "아이디와 비밀번호가 일치하지 않습니다.");
}
