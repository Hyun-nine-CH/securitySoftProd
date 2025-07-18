#include "communication.h"
#include "dialog_log.h"
#include "Protocol.h"
#include <QMessageBox>
#include <QFileInfo>

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
    connect(this, &Communication::FinishInit,this,&Communication::RequestUserInfo);
    // 기존 코드 일부 (예시)
    //connect(socket, &QTcpSocket::bytesWritten, this, &Communication::goOnSend); // Qt5 신 문법
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
            break;
        case ID_CHECK:
            QJsonDocument doc = QJsonDocument::fromJson(m_buffer);
            if(doc.object()["IdCheck"] == "unique")
                emit IdCheckResult(false);
            else
                emit IdCheckResult(true);
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

void Communication::Receive_Product(const QBuffer &buffer)
{
    if(isFirst){
        isFirst = false;
        emit FinishInit();
    }
    if(getUserInfo().value("ClientId").toInteger() > 1000){
        emit ReceiveProductInfo_ad(buffer);
    }else{
        emit ReceiveProductInfo(buffer);
    }
}

void Communication::Receive_UserInfo(const QBuffer &buffer)
{
    if(isRoomInit){
        isRoomInit = false;
        emit ReceviceChatRoomInfo(buffer);
    }else{
        emit ReceiveUserInfo(buffer);
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

void Communication::IdChekc(const QBuffer &buffer)
{
    ProcessBuffer(buffer, ID_CHECK);
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

void Communication::RequestProductInfo_ad()
{
    qDebug() << "admin 제품목록 요청 데이터 생성 중...";
    QJsonObject ProductObject;
    ProductObject["product"] = "Request Product";
    QByteArray payload = QJsonDocument(ProductObject).toJson();

    qDebug() << "admin 제품목록 요청 JSON 생성됨:" << QString(payload);

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
    qDebug() << "서버로 admin 제품목록 요청 전송 중... (프로토콜:" << Protocol::Request_Product_List << ", 크기:" << totalSize << "바이트)";

    qDebug() << "admin 제품목록 요청 전송 완료";
}

void Communication::RequestOrderInfo()
{
    qDebug() << "주문목록 요청 데이터 생성 중...";
    QJsonObject OrderObject;
    OrderObject["Order"] = "Client Order List";
    QByteArray payload = QJsonDocument(OrderObject).toJson();

    qDebug() << "주문목록 요청 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "Product info";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Request_That_Order) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 주문목록 요청 전송 중... (프로토콜:" << Protocol::Request_That_Order << ", 크기:" << totalSize << "바이트)";

    qDebug() << "주문목록 요청 전송 완료";
}

void Communication::RequestAllOrderInfo()
{
    qDebug() << "모든 주문목록 요청 데이터 생성 중...";
    QJsonObject OrderObject;
    OrderObject["Order"] = "Client Order List";
    QByteArray payload = QJsonDocument(OrderObject).toJson();

    qDebug() << "모든 주문목록 요청 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "Product info";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Request_Order_Info) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 모든 주문목록 요청 전송 중... (프로토콜:" << Protocol::Request_Order_Info << ", 크기:" << totalSize << "바이트)";

    qDebug() << "모든 주문목록 요청 전송 완료";
}

void Communication::RequestUserInfo()
{
    qDebug() << "고객목록 요청 데이터 생성 중...";
    QJsonObject OrderObject;
    OrderObject["Order"] = "Client Order List";
    QByteArray payload = QJsonDocument(OrderObject).toJson();

    qDebug() << "고객목록 요청 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "Product info";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Request_User_Info) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 고객목록 요청 전송 중... (프로토콜:" << Protocol::Request_User_Info << ", 크기:" << totalSize << "바이트)";

    qDebug() << "고객목록 요청 전송 완료";
}

void Communication::RequestProductAdd(const QJsonObject &productData)
{
    qDebug() << "제품 추가 데이터 보내는 중...";
    QByteArray payload = QJsonDocument(productData).toJson();

    qDebug() << "제품 추가 데이터 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "pd add";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Add_Product) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 제품 추가 데이터 전송 중... (프로토콜:" << Protocol::Add_Product << ", 크기:" << totalSize << "바이트)";

    qDebug() << "제품 추가 데이터 전송 완료";
}

void Communication::RequestProductDel(const QJsonObject &productData)
{
    qDebug() << "제품 삭제 데이터 보내는 중...";
    QByteArray payload = QJsonDocument(productData).toJson();

    qDebug() << "제품 삭제 데이터 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "pd del";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Delete_Product) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 제품 삭제 데이터 전송 중... (프로토콜:" << Protocol::Delete_Product << ", 크기:" << totalSize << "바이트)";

    qDebug() << "제품 삭제 데이터 전송 완료";
}

void Communication::RequestProductMod(const QJsonObject &productData)
{
    qDebug() << "제품 수정 데이터 보내는 중...";
    QByteArray payload = QJsonDocument(productData).toJson();

    qDebug() << "제품 수정 데이터 JSON 생성됨:" << QString(payload);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "pd mod";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(payload);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Update_Product) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 제품 수정 데이터 전송 중... (프로토콜:" << Protocol::Update_Product << ", 크기:" << totalSize << "바이트)";

    qDebug() << "제품 수정 데이터 전송 완료";
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

void Communication::SendChatMesg_ad(const QByteArray &mesg)
{

    qDebug() << "관리자 메시지 JSON 생성됨:" << QString(mesg);

    QByteArray blockToSend;
    QDataStream out(&blockToSend, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    QByteArray filename =  "admin mesg";
    out << qint64(0) << qint64(0) << qint64(0) <<filename;
    blockToSend.append(mesg);
    qint64 totalSize = blockToSend.size();
    out.device()->seek(0);
    out << qint64(Protocol::Chatting_Parse) << totalSize << totalSize;
    socket->write(blockToSend);
    qDebug() << "서버로 관리자 메시지 전송 중... (프로토콜:" << Protocol::Chatting_Parse << ", 크기:" << totalSize << "바이트)";

    qDebug() << "관리자 메시지 전송 완료";
}

void Communication::SendIdCheck(const QByteArray &idcheck)
{
    socket->write(idcheck);
    qDebug() << "아이디 중복 데이터 전송 완료";
}

void Communication::SendJoinData(const QByteArray &idcheck)
{
    socket->write(idcheck);
    qDebug() << "회원가입 데이터 전송 완료";
}

void Communication::SendOrderData(const QByteArray &data)
{
    socket->write(data);
    qDebug() << "주문 데이터 전송 완료";
}

void Communication::SendFile(QFile *file)
{
    isFileSending = true;
    disconnect(socket, &QTcpSocket::readyRead, this, &Communication::onReadyRead);
    connect(socket, &QTcpSocket::bytesWritten, this, &Communication::goOnSend);
    if (file->isOpen()) {
        file->close();
    }
    if (file->open(QFile::ReadOnly)) {
        Files_ = file; // 파일 객체 저장 (소유권 이전)
        QString filename = Files_->fileName(); // 파일 이름 저장
        byteToWrite = TotalSize = Files_->size();
        // 파일의 바이너리 데이터를 읽어옴
        QByteArray fileData = Files_->readAll();

        // 파일 데이터를 Base64로 인코딩
        QByteArray base64EncodedData = fileData.toBase64();

        // JSON 객체 생성
        QJsonObject jsonObject;
        jsonObject["type"] = "file_transfer"; // 메시지 타입
        jsonObject["filename"] = QFileInfo(filename).fileName(); // 파일 이름만 추출
        jsonObject["filetype"] = "image/png"; // 파일 타입 (PNG로 가정)
        jsonObject["data"] = QString::fromLatin1(base64EncodedData); // Base64 데이터를 문자열로 변환하여 저장
        jsonObject["id"] = Communication::getInstance()->getUserInfo().value("id").toString();
        // JSON 객체를 QJsonDocument로 변환
        QByteArray payload = QJsonDocument(jsonObject).toJson();

        // 전송할 데이터 크기 설정

        loadSize = 1024;
        // 헤더 정보 추가 (기존 방식과 유사하게)
        QDataStream out(&outBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_15);
        // 헤더 정보 설정 (데이터 타입, 전체 크기, 헤더 크기)
        out << qint64(0) << qint64(0) << qint64(0) << filename;
        outBlock.append(payload);
        byteToWrite += outBlock.size();
        TotalSize   += outBlock.size();

        // 헤더 정보 업데이트
        out.device()->seek(0);
        qint64 dataType = Protocol::File_Transfer; // 프로토콜에 정의된 파일 전송 타입
        out << dataType <<  TotalSize - sizeof(qint64) * 3 - filename.size() * 2  << qint64(outBlock.size());

        // 헤더 전송
        socket->write(outBlock);

        qDebug() << "File encoded as Base64 and prepared as JSON. Total size:" << TotalSize;
    } else {
        qDebug() << "Failed to open file:" << file->fileName();
        delete file;
        file = nullptr;
    }
}

void Communication::onReadyRead()
{
    if(!isFileSending)
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
        // qDebug() << "데이터 타입: " << DataType << ", 전체 크기: " << TotalSize
        //          << ", 현재 패킷: " << CurrentPacket << ", 파일명: " << FileName;
        //qDebug() << "데이터 타입 : " << DataType;
        switch (DataType) {
        //case 0x01:FileReceive            (buffer);     break;
        case 0x03:Receive_Product  (buffer);       break;
        //case 0x04:ModiProductInfo        (buffer);     break;
        //case 0x05:AddProductInfo         (buffer);     break;
        //case 0x06:DelProductInfo         (buffer);     break;
        case 0x07:Login         (buffer);     break;
        //case 0x08:SignUp                   (buffer);     break;
        case 0x09:Receive_UserInfo(buffer);     break;
        //case 0x10:AddOrderInfo           (buffer);     break;
        case 0x11:emit ReceiveAllOrderInfo  (buffer);    break;
        //case 0x12:emit RequestChatLogInfo(this);       break;
        case 0x13:emit ReceiveChat(buffer);       break;
        case 0x14:IdChekc(buffer);                break;
        case 0x15:emit ReceiveOrderInfo(buffer);  break;
        }
        m_buffer.clear();
    }
}

void Communication::goOnSend(qint64 numBytes)
{
    byteToWrite -= numBytes;
    outBlock = Files_->read(qMin(byteToWrite,numBytes));
    socket->write(outBlock);
    if(byteToWrite == 0){
        disconnect(socket, &QTcpSocket::bytesWritten, this, &Communication::goOnSend);
        connect(socket, &QTcpSocket::readyRead, this, &Communication::onReadyRead);
        isFileSending = false;
        QMessageBox::information(0,tr("파일 전송 완료"), tr("파일이 성공적으로 전송되었습니다."), QMessageBox::Ok);
    }
}

