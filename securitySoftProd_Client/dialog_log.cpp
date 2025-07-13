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

// 정적 멤버 변수 초기화
Dialog_log* Dialog_log::instance = nullptr;
QTcpSocket* Dialog_log::socket = nullptr;

Dialog_log::Dialog_log(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog_log)
{
    ui->setupUi(this);

    qDebug() << "Dialog_log 생성자 호출됨";

    // 소켓이 아직 생성되지 않았다면 생성
    if (!socket) {
        qDebug() << "새로운 소켓 생성 시작...";
        socket = new QTcpSocket(); // this를 제거하여 부모-자식 관계 없앰

        if (socket) {
            qDebug() << "소켓 생성 성공!";
        } else {
            qDebug() << "소켓 생성 실패!";
            QMessageBox::critical(this, "오류", "소켓을 생성할 수 없습니다.");
            return;
        }

        // 소켓 시그널-슬롯 연결
        connect(socket, &QTcpSocket::readyRead, this, &Dialog_log::onReadyRead);
        connect(socket, &QTcpSocket::connected, this, [](){
            qDebug() << "[Client] 서버에 연결됨!";
        });
        connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error){
            qDebug() << "[Client] 소켓 오류 발생:" << socket->errorString() << "오류 코드:" << error;
            QMessageBox::critical(this, "Connection Error", socket->errorString());
        });

        qDebug() << "서버 연결 시도 중... (127.0.0.1:50000)";
        socket->connectToHost("127.0.0.1", 50000);
    }

    // --- UI/UX 개선 기능 추가 ---
    // 1. 비밀번호 입력란 마스킹 처리
    ui->lineEdit_pw->setEchoMode(QLineEdit::Password);
    // 2. 비밀번호 입력란에 이벤트 필터 설치 (Enter 키 감지용)
    ui->lineEdit_pw->installEventFilter(this);

    // 버튼 시그널-슬롯 연결
    connect(ui->pushButton_login, &QPushButton::clicked, this, &Dialog_log::on_pushButton_login_clicked);
    connect(ui->pushButton_signUp, &QPushButton::clicked, this, &Dialog_log::on_pushButton_signUp_clicked);

    qDebug() << "Dialog_log 초기화 완료";
}

Dialog_log::~Dialog_log() {
    qDebug() << "Dialog_log 소멸자 호출됨";
    delete ui;
    instance = nullptr;
    // 소켓은 삭제하지 않음 - 프로그램 종료 시까지 유지
}

Dialog_log* Dialog_log::getInstance(QWidget *parent) {
    if (!instance) {
        qDebug() << "Dialog_log 싱글톤 인스턴스 최초 생성";
        instance = new Dialog_log(parent);
    } else {
        qDebug() << "Dialog_log 싱글톤 인스턴스 재사용";
    }
    return instance;
}

// Enter 키를 누르면 로그인 버튼이 클릭되도록 하는 이벤트 필터
bool Dialog_log::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->lineEdit_pw && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            qDebug() << "비밀번호 입력 후 Enter 키 감지됨 - 로그인 시도";
            on_pushButton_login_clicked();
            return true; // 이벤트 처리 완료
        }
    }
    return QDialog::eventFilter(watched, event);
}

void Dialog_log::on_pushButton_login_clicked() {
    QString id = ui->lineEdit_id->text().trimmed();
    QString pw = ui->lineEdit_pw->text().trimmed();

    qDebug() << "로그인 버튼 클릭됨 - ID:" << id;

    if (id.isEmpty() || pw.isEmpty()) {
        qDebug() << "ID 또는 비밀번호가 비어있음 - 로그인 중단";
        QMessageBox::warning(this, "Login", "Please enter both ID and password.");
        return;
    }

    qDebug() << "로그인 요청 데이터 생성 중...";
    QJsonObject loginObject;
    loginObject["id"] = id;
    loginObject["pw"] = pw;
    QByteArray payload = QJsonDocument(loginObject).toJson();

    qDebug() << "로그인 요청 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << qint64(0) << qint64(0) << qint64(0) << "login info";
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Login_Request) << totalSize << totalSize;

    qDebug() << "서버로 로그인 요청 전송 중... (프로토콜:" << Protocol::Login_Request << ", 크기:" << totalSize << "바이트)";
    socket->write(blockToSend);
    socket->flush();
    qDebug() << "로그인 요청 전송 완료";
}

void Dialog_log::on_pushButton_signUp_clicked() {
    qDebug() << "회원가입 버튼 클릭됨 - Dialog_SignUp 생성";
    Dialog_SignUp signUpDialog(socket, this); // 소켓 포인터 전달
    qDebug() << "회원가입 다이얼로그 실행";
    signUpDialog.exec();
    qDebug() << "회원가입 다이얼로그 종료";
}

void Dialog_log::onReadyRead() {
    qDebug() << "서버로부터 데이터 수신 시작";
    m_buffer.append(socket->readAll());
    qDebug() << "현재 버퍼 크기:" << m_buffer.size() << "바이트";

    while(true) {
        QDataStream in(&m_buffer, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_15);

        // 헤더 크기 확인
        if (m_buffer.size() < (3 * sizeof(qint64))) {
            qDebug() << "헤더 크기 부족 - 더 많은 데이터 대기 중";
            break;
        }

        qint64 dataType, totalSize, currentPacketSize;
        in >> dataType >> totalSize >> currentPacketSize;
        qDebug() << "수신된 패킷 정보 - 데이터 타입:" << dataType << ", 전체 크기:" << totalSize << "바이트";

        // 전체 패킷 크기 확인
        if (m_buffer.size() < totalSize) {
            qDebug() << "패킷 데이터 부족 - 현재:" << m_buffer.size() << "바이트, 필요:" << totalSize << "바이트";
            break;
        }

        QString filename;
        in >> filename;
        qDebug() << "파일명:" << filename;

        // 페이로드 추출
        QByteArray payload = m_buffer.mid(in.device()->pos(), totalSize - in.device()->pos());
        qDebug() << "JSON 페이로드 크기:" << payload.size() << "바이트";

        // JSON 파싱
        QJsonDocument doc = QJsonDocument::fromJson(payload);
        qDebug() << "JSON 파싱 완료";

        if (doc.isObject() && !doc.object().isEmpty()) {
            qDebug() << "유효한 JSON 객체 수신됨 - 로그인 성공!";

            // 사용자 정보 저장
            m_userInfo = doc.object();

            // 수신된 JSON 데이터의 모든 키-값 쌍 출력
            qDebug() << "===== 수신된 사용자 정보 =====";
            for (auto it = m_userInfo.begin(); it != m_userInfo.end(); ++it) {
                qDebug() << it.key() << ":" << it.value().toVariant();
            }
            qDebug() << "=============================";

            // 주요 정보 로그 출력
            qDebug() << "ClientId:" << m_userInfo["ClientId"].toInt();
            qDebug() << "사용자 ID:" << m_userInfo["id"].toString();
            qDebug() << "소속:" << m_userInfo["RoomId"].toString();
            qDebug() << "부서:" << m_userInfo["department"].toString();
            qDebug() << "관리자:" << m_userInfo["manager"].toString();

            qDebug() << "사용자 정보를 싱글톤 객체에 저장 완료";
            qDebug() << "로그인 다이얼로그 accept() 호출 - 메인 윈도우로 전환 예정";
            this->accept();
        } else {
            qDebug() << "빈 JSON 객체 또는 파싱 실패 - 로그인 실패";
            QMessageBox::warning(this, "로그인 실패", "아이디 또는 비밀번호가 일치하지 않습니다.");
        }

        // 처리 완료된 데이터 제거
        m_buffer.remove(0, totalSize);
        qDebug() << "처리된 데이터 제거 후 버퍼 크기:" << m_buffer.size() << "바이트";
    }
}

QJsonObject Dialog_log::getUserInfo() const {
    qDebug() << "getUserInfo() 호출됨 - 저장된 사용자 정보 반환";
    return m_userInfo;
}

QTcpSocket* Dialog_log::getSocket() {
    qDebug() << "getSocket() 호출됨 - 싱글톤 소켓 포인터 반환";
    return socket;
}
