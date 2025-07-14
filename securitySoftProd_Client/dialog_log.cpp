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
#include "Protocol.h"
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
    connect(this,Communication::LoginSuccess(),)
}
