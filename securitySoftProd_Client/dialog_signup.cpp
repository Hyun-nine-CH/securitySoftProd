#include "dialog_signup.h"
#include "ui_dialog_signup.h"
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>
#include <QDebug>
#include "Protocol.h"
#include "communication.h"

Dialog_SignUp::Dialog_SignUp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_SignUp),
    m_idChecked(false),
    m_pwChecked(false)
{
    ui->setupUi(this);

    qDebug() << "회원가입 다이얼로그 생성됨";

    // 비밀번호 입력란에 마스킹 처리
    ui->PW->setEchoMode(QLineEdit::Password);
    ui->PW_check->setEchoMode(QLineEdit::Password);

    // 버튼과 슬롯 연결
    //connect(ui->pushButton_SignUp, &QPushButton::clicked, this, &Dialog_SignUp::on_pushButton_SignUp_clicked);
    //connect(ui->pushButton_IDcheck, &QPushButton::clicked, this, &Dialog_SignUp::on_pushButton_IDcheck_clicked);
    //connect(ui->pushButton_DoubleCheck, &QPushButton::clicked, this, &Dialog_SignUp::on_pushButton_DoubleCheck_clicked);
    connect(Communication::getInstance(),&Communication::IdCheckResult,this,&Dialog_SignUp::IdCheck);
}

Dialog_SignUp::~Dialog_SignUp()
{
    delete ui;
}

void Dialog_SignUp::on_pushButton_IDcheck_clicked()
{
    qDebug() << "ID 중복 확인 버튼 클릭됨";

    QString id = ui->ID->text().trimmed();

    if (id.isEmpty()) {
        QMessageBox::warning(this, QObject::tr("입력 오류"), QObject::tr("ID를 입력해주세요."));
        return;
    }

    // ID 중복 확인 요청 데이터 생성
    QJsonObject idCheckObject;
    idCheckObject["id"] = id;

    QByteArray payload = QJsonDocument(idCheckObject).toJson();
    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray str = "id_check";
    out << qint64(0) << qint64(0) << qint64(0) << str;
    blockToSend.append(payload);

    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::ID_Check_Request) << totalSize << totalSize;

    qDebug() << "서버로 ID 중복 확인 요청 전송 중... ID:" << id;

    Communication::getInstance()->SendIdCheck(blockToSend);
}

void Dialog_SignUp::on_pushButton_DoubleCheck_clicked()
{
    qDebug() << "비밀번호 확인 버튼 클릭됨";

    QString pw = ui->PW->text();
    QString pw_check = ui->PW_check->text();

    if (pw.isEmpty() || pw_check.isEmpty()) {
        QMessageBox::warning(this, QObject::tr("입력 오류"), QObject::tr("비밀번호를 모두 입력해주세요."));
        return;
    }

    if (pw == pw_check) {
        QMessageBox::information(this, QObject::tr("확인 완료"), QObject::tr("비밀번호가 일치합니다."));
        m_pwChecked = true;
        qDebug() << "비밀번호 확인 완료: 일치함";
    } else {
        QMessageBox::warning(this, QObject::tr("입력 오류"), QObject::tr("비밀번호가 일치하지 않습니다."));
        m_pwChecked = false;
        qDebug() << "비밀번호 확인 실패: 불일치";
    }
}

void Dialog_SignUp::IdCheck(bool isDupli)
{
    if(isDupli){
        //중복이다
        QMessageBox::warning(this, QObject::tr("중복 확인"), QObject::tr("아이디가 중복입니다"));
        m_idChecked = false;
    }else{
        //중복이 아니다
        QMessageBox::information(this, QObject::tr("중복 확인"), QObject::tr("사용가능한 아이디 입니다"));
        m_idChecked = true;
    }

}

void Dialog_SignUp::on_pushButton_SignUp_clicked()
{
    qDebug() << "회원가입 버튼 클릭됨";

    // 1. UI의 각 입력란에서 사용자 정보 가져오기
    QString company = ui->CompanyName->text().trimmed();
    QString department = ui->Dep_Name->text().trimmed();
    QString manager = ui->Client_Name->text().trimmed();
    QString phone = ui->Phone_Num->text().trimmed();
    QString address = ui->Address->text().trimmed();
    QString id = ui->ID->text().trimmed();
    QString pw = ui->PW->text();
    QString pw_check = ui->PW_check->text();

    // 2. 유효성 검사
    if (company.isEmpty() || department.isEmpty() || manager.isEmpty() || id.isEmpty() || pw.isEmpty()) {
        QMessageBox::warning(this, QObject::tr("입력 오류"), QObject::tr("필수 항목을 모두 입력해주세요."));
        qDebug() << "회원가입 실패: 필수 항목 누락";
        return;
    }

    // ID 중복 확인 여부 검사
    if (!m_idChecked) {
        QMessageBox::warning(this, QObject::tr("ID 확인 필요"), QObject::tr("ID 중복 확인을 먼저 진행해주세요."));
        qDebug() << "회원가입 실패: ID 중복 확인 안됨";
        return;
    }

    // 비밀번호 확인 여부 검사
    if (!m_pwChecked) {
        QMessageBox::warning(this, QObject::tr("비밀번호 확인 필요"), QObject::tr("비밀번호 확인을 먼저 진행해주세요."));
        qDebug() << "회원가입 실패: 비밀번호 확인 안됨";
        return;
    }

    // 3. 서버의 ClientInfo.json 형식에 맞춰 QJsonObject 생성
    QJsonObject newUserObject;
    newUserObject["id"] = id;
    newUserObject["pw"] = pw;
    newUserObject["RoomId"] = company; // 'company'가 'RoomId' 역할을 함
    newUserObject["department"] = department;
    newUserObject["manager"] = manager;
    newUserObject["phone"] = phone;
    newUserObject["address"] = address;

    qDebug() << "회원가입 요청 JSON 생성됨:" << QJsonDocument(newUserObject).toJson();

    // 4. 서버 프로토콜에 맞춰 전송할 데이터(block) 생성
    QByteArray payload = QJsonDocument(newUserObject).toJson();
    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray title = "join_request";
    out << qint64(0) << qint64(0) << qint64(0) << title;
    blockToSend.append(payload);

    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Join_Request) << totalSize << totalSize;
    Communication::getInstance()->SendJoinData(blockToSend);

    QMessageBox::information(this, QObject::tr("회원가입"), QObject::tr("회원가입이 완료 되었습니다"));
    this->close();
}
