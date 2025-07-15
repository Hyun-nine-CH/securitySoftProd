#include "communication.h"
#include "dialog_log.h"
#include "Protocol.h"
#include <QMessageBox>

Communication* Communication::instance = nullptr; // 정적 멤버 초기화

Communication* Communication::getInstance() {
    if (!instance) {
        instance = new Communication();
    }else {
        //qDebug() << "싱글톤 인스턴스 재사용";
    }
    return instance;
}

Communication::~Communication()
{

}

QJsonObject Communication::getUserInfo() const
{
    return m_userInfo;
}

Communication::Communication()
{
    socket = new QTcpSocket(this);

    TotalSize     = 0;
    CurrentPacket = 0;
    DataType      = 0;
    ReceivePacket = 0;
    m_buffer      = 0;

    // 소켓 시그널-슬롯 연결
    connect(socket, &QTcpSocket::readyRead, this, &Communication::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &Communication::StartComm);
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error){
        qDebug() << "[Client] 소켓 오류 발생:" << socket->errorString() << "오류 코드:" << error;
        QMessageBox::critical(0, "Connection Error", socket->errorString());
    });

    qDebug() << "서버 연결 시도 중... (127.0.0.1:50000)";
    socket->connectToHost("127.0.0.1", 50000);
}

void Communication::ProcessBuffer(const QBuffer &buffer, int requestType)
{
    if(ReceivePacket == 0){
        m_buffer.remove(0, buffer.pos());
        ReceivePacket = CurrentPacket;
        qDebug() << "ReceivePacket : " << ReceivePacket;
        qDebug() << "TotalSize : " << TotalSize;
    }else{
        qDebug() << " 내용 : " << m_buffer;
        ReceivePacket += m_buffer.size();
        qDebug() << "ReceivePacket : " << ReceivePacket;
        qDebug() << "TotalSize : " << TotalSize;
    }

    if(ReceivePacket == TotalSize){
        // 요청 타입에 따라 다른 메시지와 시그널 발생
        switch(requestType) {
        case LOGIN: // ModiProductInfo
            if(TotalSize > 10){
                qDebug() << "Login Success";
                setUserInfo(m_buffer);
                emit LoginSuccess();
            }
            else{
                qDebug() << "Login Fail";
                emit LoginFail();
            }

            //qDebug() << "User Info : " << m_buffer;
            //emit ModifyProductDB(this, ByteArray);
            break;
        }

        // 공통 초기화 코드
        ReceivePacket = 0;
        TotalSize = 0;
        DataType = 0;
        m_buffer.clear();
    }
}

void Communication::setUserInfo(const QByteArray &buffer)
{
    // JSON 파싱
    QJsonDocument doc = QJsonDocument::fromJson(buffer);
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
    } else {
        qDebug() << "빈 JSON 객체 또는 파싱 실패 - 로그인 실패";
        QMessageBox::warning(0, "로그인 실패", "아이디 또는 비밀번호가 일치하지 않습니다.");
    }

}

void Communication::SendLoginConfirm(const QString &id, const QString &pw)
{
    qDebug() << "로그인 버튼 클릭됨 - ID:" << id;

    if (id.isEmpty() || pw.isEmpty()) {
        qDebug() << "ID 또는 비밀번호가 비어있음 - 로그인 중단";
        QMessageBox::warning(0, "Login", "Please enter both ID and password.");
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
    QByteArray filename =  "login info";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Login_Request) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 로그인 요청 전송 중... (프로토콜:" << Protocol::Login_Request << ", 크기:" << totalSize << "바이트)";

    qDebug() << "로그인 요청 전송 완료";
}

void Communication::Login(const QBuffer &buffer)
{
    ProcessBuffer(buffer, LOGIN);
}

void Communication::StartComm()
{
    qDebug() << "서버연결 완료";
}

QTcpSocket *Communication::getSocket()
{
    return socket;
}

void Communication::RequestProductInfo()
{
    qDebug() << "제품목록 요청 데이터 생성 중...";
    QJsonObject ProductObject;
    ProductObject["product"] = "Request Product";
    QByteArray payload = QJsonDocument(ProductObject).toJson();

    qDebug() << "제품목록 요청 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "Product info";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Request_Product_List) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 제품목록 요청 전송 중... (프로토콜:" << Protocol::Request_Product_List << ", 크기:" << totalSize << "바이트)";

    qDebug() << "제품목록 요청 전송 완료";
}

void Communication::SendChatMesg(const QString &mesg)
{
    qDebug() << "메시지 보내는 중...";
    QJsonObject ChatObject;
    ChatObject["message"] = mesg;
    ChatObject["nickname"] = getUserInfo()["id"].toString();
    QByteArray payload = QJsonDocument(ChatObject).toJson();

    qDebug() << "메시지 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "mesg";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Chatting_Parse) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 메시지 전송 중... (프로토콜:" << Protocol::Chatting_Parse << ", 크기:" << totalSize << "바이트)";

    qDebug() << "메시지 전송 완료";
}

void Communication::onReadyRead()
{
    //qDebug() << "수신 전 ByteArray 크기: " << ByteArray.size();
    m_buffer.append(socket->readAll());
    qDebug() << "수신 후 ByteArray 크기: " << m_buffer.size();
    QBuffer buffer(&m_buffer);
    buffer.open(QIODevice::ReadOnly); // 읽기 모드로 오픈 필수
    QDataStream In(&buffer);
    In.setVersion(QDataStream::Qt_5_15);
    qDebug() <<"받았을때 시점의 내용 : " << m_buffer;
    // qDebug() << "Server: " << ClientConnection->peerAddress().toString()
    //          << "에서 데이터 수신. 현재 버퍼 크기: " << ByteArray.size();
    QByteArray FileName;
    if(ReceivePacket == 0)
        In >> DataType >> TotalSize >> CurrentPacket >> FileName;
    qDebug() << "데이터 타입: " << DataType << ", 전체 크기: " << TotalSize
             << ", 현재 패킷: " << CurrentPacket << ", 파일명: " << FileName;
    //qDebug() << "데이터 타입 : " << DataType;
    switch (DataType) {
    //case 0x01:FileReceive            (buffer);     break;
    case 0x03:emit ReceiveProductInfo  (buffer);       break;
    //case 0x04:ModiProductInfo        (buffer);     break;
    //case 0x05:AddProductInfo         (buffer);     break;
    //case 0x06:DelProductInfo         (buffer);     break;
    case 0x07:Login         (buffer);     break;
    //case 0x08:Join                   (buffer);     break;
    //case 0x09:emit RequestUserInfo   (this);       break;
    //case 0x10:AddOrderInfo           (buffer);     break;
    //case 0x11:emit RequestOrderInfo  (this);       break;
    //case 0x12:emit RequestChatLogInfo(this);       break;
    case 0x13:emit ReceiveChat(buffer);       break;
    }
    m_buffer.clear();
}

