#include "dialog_signup.h"
#include "ui_dialog_signup.h"
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>

// 서버와 통신할 프로토콜 정의
namespace Protocol {
enum DataType : qint64 {
    Join_Request = 0x08, // 서버의 'Join' 케이스에 해당
};
}

Dialog_SignUp::Dialog_SignUp(QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_SignUp),
    m_socket(socket) // 전달받은 소켓을 멤버 변수에 저장
{
    ui->setupUi(this);

    // 비밀번호 확인 입력란에 마스킹 처리
    ui->PW->setEchoMode(QLineEdit::Password);
    ui->PW_check->setEchoMode(QLineEdit::Password);

    // objectName이 'pushButton_SignUp'인 버튼과 슬롯을 자동 연결
    connect(ui->pushButton_SignUp, &QPushButton::clicked, this, &Dialog_SignUp::on_pushButton_SignUp_clicked);
}

Dialog_SignUp::~Dialog_SignUp()
{
    delete ui;
}

void Dialog_SignUp::on_pushButton_SignUp_clicked()
{
    // 1. UI의 각 입력란(objectName 기준)에서 사용자 정보 가져오기
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
        QMessageBox::warning(this, "입력 오류", "필수 항목을 모두 입력해주세요.");
        return;
    }
    if (pw != pw_check) {
        QMessageBox::warning(this, "입력 오류", "비밀번호가 일치하지 않습니다.");
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
    // ClientId는 서버에서 자동으로 부여하므로 보내지 않음

    // 4. 서버 프로토콜에 맞춰 전송할 데이터(block) 생성
    QByteArray payload = QJsonDocument(newUserObject).toJson();
    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << qint64(0) << qint64(0) << qint64(0) << "join_request";
    blockToSend.append(payload);

    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Join_Request) << totalSize << totalSize;

    // 5. 소켓으로 데이터 전송
    if (m_socket && m_socket->isOpen()) {
        m_socket->write(blockToSend);
        m_socket->flush();
    }

    // 6. 사용자에게 알리고 창 닫기
    QMessageBox::information(this, "회원가입 신청", "회원가입이 신청되었습니다.\n로그인 창에서 다시 로그인해주세요.");
    this->accept(); // 창을 '성공' 상태로 닫음
}
