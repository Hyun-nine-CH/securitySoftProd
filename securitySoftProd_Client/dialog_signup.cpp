#include "dialog_signup.h"
#include "ui_dialog_signup.h"
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>
#include <QDebug>
#include "Protocol.h"

Dialog_SignUp::Dialog_SignUp(QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_SignUp),
    m_socket(socket),
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

    // 소켓 시그널 연결 - 서버 응답 처리
    // if (m_socket) {
    //     connect(m_socket, &QTcpSocket::readyRead, this, &Dialog_SignUp::onReadyRead);
    //     qDebug() << "회원가입 다이얼로그에서 소켓 readyRead 시그널 연결됨";
    // } else {
    //     qDebug() << "경고: 회원가입 다이얼로그에 전달된 소켓이 nullptr입니다!";
    // }
}

Dialog_SignUp::~Dialog_SignUp()
{
    qDebug() << "회원가입 다이얼로그 소멸자 호출됨";

    // 소켓 시그널 연결 해제 (Dialog_log의 onReadyRead가 처리하도록)
    if (m_socket) {
        disconnect(m_socket, &QTcpSocket::readyRead, this, &Dialog_SignUp::onReadyRead);
        qDebug() << "회원가입 다이얼로그에서 소켓 readyRead 시그널 연결 해제됨";
    }

    delete ui;
}

void Dialog_SignUp::on_pushButton_IDcheck_clicked()
{
    qDebug() << "ID 중복 확인 버튼 클릭됨";

    QString id = ui->ID->text().trimmed();

    if (id.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "ID를 입력해주세요.");
        return;
    }

    // ID 중복 확인 요청 데이터 생성
    QJsonObject idCheckObject;
    idCheckObject["id"] = id;

    QByteArray payload = QJsonDocument(idCheckObject).toJson();
    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << qint64(0) << qint64(0) << qint64(0) << "id_check";
    blockToSend.append(payload);

    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::ID_Check_Request) << totalSize << totalSize;

    qDebug() << "서버로 ID 중복 확인 요청 전송 중... ID:" << id;

    // // 소켓으로 데이터 전송
    // if (m_socket && m_socket->isOpen()) {
    //     m_socket->write(blockToSend);
    //     m_socket->flush();
    //     qDebug() << "ID 중복 확인 요청 전송 완료";
    // } else {
    //     qDebug() << "오류: 소켓이 열려있지 않아 ID 중복 확인 요청을 보낼 수 없습니다.";
    //     QMessageBox::critical(this, "연결 오류", "서버와 연결이 끊어졌습니다.");
    // }

    // 서버 응답은 onReadyRead()에서 처리
}

void Dialog_SignUp::on_pushButton_DoubleCheck_clicked()
{
    qDebug() << "비밀번호 확인 버튼 클릭됨";

    QString pw = ui->PW->text();
    QString pw_check = ui->PW_check->text();

    if (pw.isEmpty() || pw_check.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "비밀번호를 모두 입력해주세요.");
        return;
    }

    if (pw == pw_check) {
        QMessageBox::information(this, "확인 완료", "비밀번호가 일치합니다.");
        m_pwChecked = true;
        qDebug() << "비밀번호 확인 완료: 일치함";
    } else {
        QMessageBox::warning(this, "입력 오류", "비밀번호가 일치하지 않습니다.");
        m_pwChecked = false;
        qDebug() << "비밀번호 확인 실패: 불일치";
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
        QMessageBox::warning(this, "입력 오류", "필수 항목을 모두 입력해주세요.");
        qDebug() << "회원가입 실패: 필수 항목 누락";
        return;
    }

    // ID 중복 확인 여부 검사
    if (!m_idChecked) {
        QMessageBox::warning(this, "ID 확인 필요", "ID 중복 확인을 먼저 진행해주세요.");
        qDebug() << "회원가입 실패: ID 중복 확인 안됨";
        return;
    }

    // 비밀번호 확인 여부 검사
    if (!m_pwChecked) {
        QMessageBox::warning(this, "비밀번호 확인 필요", "비밀번호 확인을 먼저 진행해주세요.");
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
    // ClientId는 서버에서 자동으로 부여하므로 보내지 않음

    qDebug() << "회원가입 요청 JSON 생성됨:" << QJsonDocument(newUserObject).toJson();

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
    // if (m_socket && m_socket->isOpen()) {
    //     qDebug() << "서버로 회원가입 요청 전송 중... (프로토콜:" << Protocol::Join_Request << ", 크기:" << totalSize << "바이트)";
    //     m_socket->write(blockToSend);
    //     m_socket->flush();
    //     qDebug() << "회원가입 요청 전송 완료";

    //     // 서버 응답은 onReadyRead()에서 처리
    //     // 여기서는 바로 창을 닫지 않고, 서버 응답을 기다림
    //     QMessageBox::information(this, "처리 중", "회원가입 요청을 처리 중입니다. 잠시만 기다려주세요.");
    // } else {
    //     qDebug() << "오류: 소켓이 열려있지 않아 회원가입 요청을 보낼 수 없습니다.";
    //     QMessageBox::critical(this, "연결 오류", "서버와 연결이 끊어졌습니다.");
    // }
}

void Dialog_SignUp::onReadyRead()
{
    qDebug() << "회원가입 다이얼로그: 서버로부터 데이터 수신 시작";
    //m_buffer.append(m_socket->readAll());
    qDebug() << "현재 버퍼 크기:" << m_buffer.size() << "바이트";

    // while(true) {
    //     QDataStream in(&m_buffer, QIODevice::ReadOnly);
    //     in.setVersion(QDataStream::Qt_5_15);

    //     // 헤더 크기 확인
    //     if (m_buffer.size() < (3 * sizeof(qint64))) {
    //         qDebug() << "헤더 크기 부족 - 더 많은 데이터 대기 중";
    //         break;
    //     }

    //     qint64 dataType, totalSize, currentPacketSize;
    //     in >> dataType >> totalSize >> currentPacketSize;
    //     qDebug() << "수신된 패킷 정보 - 데이터 타입:" << dataType << ", 전체 크기:" << totalSize << "바이트";

    //     // 전체 패킷 크기 확인
    //     if (m_buffer.size() < totalSize) {
    //         qDebug() << "패킷 데이터 부족 - 현재:" << m_buffer.size() << "바이트, 필요:" << totalSize << "바이트";
    //         break;
    //     }

    //     QString filename;
    //     in >> filename;
    //     qDebug() << "파일명:" << filename;

    //     // 페이로드 추출
    //     QByteArray payload = m_buffer.mid(in.device()->pos(), totalSize - in.device()->pos());
    //     qDebug() << "JSON 페이로드 크기:" << payload.size() << "바이트";
    //     // JSON 파싱
    //     QJsonDocument doc = QJsonDocument::fromJson(payload);
    //     qDebug() << "JSON 파싱 완료";

    //     if (doc.isObject()) {
    //         QJsonObject responseObj = doc.object();

    //         // 응답 타입에 따라 처리
    //         if (dataType == Protocol::ID_Check_Request) {
    //             // ID 중복 확인 응답 처리
    //             bool isAvailable = responseObj["available"].toBool();

    //             if (isAvailable) {
    //                 QMessageBox::information(this, "ID 확인", "사용 가능한 ID입니다.");
    //                 m_idChecked = true;
    //                 qDebug() << "ID 중복 확인 성공: 사용 가능";
    //             } else {
    //                 QMessageBox::warning(this, "ID 확인", "이미 사용 중인 ID입니다. 다른 ID를 입력해주세요.");
    //                 m_idChecked = false;
    //                 qDebug() << "ID 중복 확인 실패: 이미 사용 중";
    //             }
    //         }
    //         else if (dataType == Protocol::Join_Request) {
    //             // 회원가입 요청 응답 처리
    //             bool success = responseObj["success"].toBool();

    //             if (success) {
    //                 QMessageBox::information(this, "회원가입 완료", "회원가입이 성공적으로 완료되었습니다.\n로그인 창에서 로그인해주세요.");
    //                 qDebug() << "회원가입 성공: 창 닫기";
    //                 this->accept(); // 창을 '성공' 상태로 닫음
    //             } else {
    //                 QString errorMsg = responseObj["message"].toString();
    //                 if (errorMsg.isEmpty()) {
    //                     errorMsg = "회원가입 처리 중 오류가 발생했습니다.";
    //                 }
    //                 QMessageBox::warning(this, "회원가입 실패", errorMsg);
    //                 qDebug() << "회원가입 실패:" << errorMsg;
    //             }
    //         }
    //     } else {
    //         qDebug() << "JSON 파싱 실패 또는 빈 객체";
    //     }

    //     // 처리 완료된 데이터 제거
    //     m_buffer.remove(0, totalSize);
    //     qDebug() << "처리된 데이터 제거 후 버퍼 크기:" << m_buffer.size() << "바이트";
    // }
}
