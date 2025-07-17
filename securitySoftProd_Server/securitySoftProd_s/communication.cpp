#include "communication.h"

#include <QFileInfo>

CommuniCation::CommuniCation(QTcpSocket* socket, ClientInfo* myInfo,QObject* parent)
    :Socket(socket), CInfo(myInfo),WorkThread(this)
{// 중요: 소켓을 이 스레드로 이동시켜, 소켓 관련 시그널/슬롯이 이 스레드에서 실행되도록 합니다.
    if (Socket) {
        //CInfo->setClientSocket(Socket);

        TotalSize     = 0;
        CurrentPacket = 0;
        DataType      = 0;
        ReceivePacket = 0;
        ByteArray     = 0;
        FileName      = 0;

        connect(Socket, SIGNAL(disconnected()),this, SLOT(ClientDisconnected()));
        connect(Socket, SIGNAL(readyRead()),SLOT(ReadClientData()));
        connect(this, &QThread::finished, Socket, &QTcpSocket::deleteLater); // 스레드 종료 시 소켓 삭제
    }
}

void CommuniCation::run()
{
    // 이 스레드의 이벤트 루프를 시작합니다.
    // m_socket->moveToThread(this)를 했기 때문에 readyRead 등의 시그널이 이 스레드에서 처리됩니다.
    exec(); // 이벤트 루프 실행 (disconnected 시그널 수신 시 종료될 수 있음)
    // exec()가 종료된 후 (예: QThread::quit() 호출 또는 이벤트 루프 종료 시)
    qDebug() << "ClientWorkerThread run() finished for socket:" << (Socket ? Socket->peerAddress().toString() : "N/A");
}

// 공통 처리 메서드 추가
void CommuniCation::ProcessBuffer(const QBuffer &buffer, int requestType)
{
    if(ReceivePacket == 0){
        ByteArray.remove(0, buffer.pos());
        ReceivePacket = CurrentPacket;
        qDebug() << "ReceivePacket : " << ReceivePacket;
        qDebug() << "TotalSize : " << TotalSize;
    }else{
        qDebug() << "chat 내용 : " << ByteArray;
        ReceivePacket += ByteArray.size();
        qDebug() << "ReceivePacket : " << ReceivePacket;
        qDebug() << "TotalSize : " << TotalSize;
    }

    if(ReceivePacket == TotalSize){
        // 요청 타입에 따라 다른 메시지와 시그널 발생
        switch(requestType) {
        case MODI_PRODUCT: // ModiProductInfo
            qDebug() << "product modify data receive completed";
            qDebug() << "end 내용 : " << ByteArray;
            emit ModifyProductDB(this, ByteArray);
            break;
        case ADD_PRODUCT: // AddProductInfo
            qDebug() << "product add data receive completed";
            emit RequestPdAdd(this, buffer);
            break;
        case DELETE_PRODUCT: // DelProductInfo
            qDebug() << "product del data receive completed";
            emit RequestPdDel(this, buffer);
            break;
        case CONFIRM_LOGIN: // ConfrimLogin
            qDebug() << "Client login data receive completed";
            emit RequestConfirm(this, buffer);
            break;
        case JOIN: // Join
            qDebug() << "Client Join data receive completed";
            emit RequestJoin(buffer);
            break;
        case ADD_ORDER: // AddOrderInfo
            qDebug() << "Order Add data receive completed";
            emit RequestOrderAdd(this, buffer);
            break;
        case CHAT_MESG: // AddOrderInfo
            qDebug() << "chat message receive completed";
            emit ChattingMesg(buffer,getClientInfo());
            break;
        case ID_CHECK:
            qDebug() << "id check data receive completed";
            emit RequestIdCheck(this,buffer);
            break;
        }
        // 공통 초기화 코드
        ReceivePacket = 0;
        TotalSize = 0;
        DataType = 0;
        ByteArray.clear();
    }
}

void CommuniCation::ReadClientData()
{
    /*
    특정 슬롯을 호출한 시그널의 발신자가 QTcpSocket 타입인지
    런타임에 안전하게 확인하고, 그 QTcpSocket 객체의 포인터를
    얻기 위한 표준적인 Qt 패턴
    시그널로 받은 그 클라이언트의 메시지를 받는 것
    */
    QTcpSocket *ClientConnection = dynamic_cast<QTcpSocket*>(sender());
    //qDebug() << "수신 전 ByteArray 크기: " << ByteArray.size();
    ByteArray.append(ClientConnection->readAll());
    qDebug() << "수신 후 ByteArray 크기: " << ByteArray.size();
    QBuffer buffer(&ByteArray);
    buffer.open(QIODevice::ReadOnly); // 읽기 모드로 오픈 필수
    QDataStream In(&buffer);
    In.setVersion(QDataStream::Qt_5_15);
    qDebug() <<"받았을때 시점의 내용 : " << ByteArray;
    // qDebug() << "Server: " << ClientConnection->peerAddress().toString()
    //          << "에서 데이터 수신. 현재 버퍼 크기: " << ByteArray.size();
    //파싱
    if(ReceivePacket == 0)
        In >> DataType >> TotalSize >> CurrentPacket >> FileName;
    // buffer의 읽기 포인터는 자동으로 8바이트 이동합니다.
    // qDebug() << "데이터 타입: " << DataType << ", 전체 크기: " << TotalSize
    //          << ", 현재 패킷: " << CurrentPacket << ", 파일명: " << FileName;
    qDebug() << "데이터 타입 : " << DataType;
    switch (DataType) {
    case 0x01:FileReceive            (buffer);                  break;
    case 0x03:emit RequestPdInfo     (this);                    break;
    case 0x04:ModiProductInfo        (buffer);                  break;
    case 0x05:AddProductInfo         (buffer);                  break;
    case 0x06:DelProductInfo         (buffer);                  break;
    case 0x07:ConfrimLogin           (buffer);                  break;
    case 0x08:Join                   (buffer);                  break;
    case 0x09:emit RequestUserInfo   (this);                    break;
    case 0x10:AddOrderInfo           (buffer);                  break;
    case 0x11:emit RequestOrderInfo  (this);                    break;
    case 0x12:emit RequestChatLogInfo(this);                    break;
    case 0x13:ChattingParse          (buffer);                  break;
    case 0x14:DuplicIdCheck          (buffer);                  break;
    case 0x15:emit RequestThatOrder  (this);    break;
    }
    ByteArray.clear();
}

void CommuniCation::FileReceive(const QBuffer &buffer)
{
    if(ReceivePacket == 0){
        QFileInfo info(FileName);
        QString CurrentFileName = info.fileName();
        NewFile = new QFile(CurrentFileName);
        NewFile->open(QFile::WriteOnly);

        // 첫 패킷에서도 헤더 제거 후 데이터를 파일에 써야 함!
        ByteArray.remove(0, buffer.pos());
        ReceivePacket = CurrentPacket; // 첫 패킷의 데이터 크기 업데이트
        //qDebug() << "첫 패킷 ReceivePacket : " << ReceivePacket;
    }else{
        qDebug() << "ByteArray 파일 : " << ByteArray;
        if (NewFile && NewFile->isOpen()) { // NewFile이 nullptr이 아니고, 열려있는지 확인
            NewFile->write(ByteArray);
        }
        ReceivePacket += ByteArray.size();
        // qDebug() << "ReceivePacket : " << ReceivePacket;
        // qDebug() << "TotalSize : " << TotalSize;
    }
    if(ReceivePacket == TotalSize){
        qDebug() <<" receive completed :" <<FileName;
        ByteArray.clear();
        ReceivePacket = 0;
        TotalSize = 0;
        DataType = 0;
        NewFile->close();
        delete NewFile;
        NewFile = nullptr;
    }
}

QSharedPointer<ClientInfo> CommuniCation::getClientInfo()
{
    return CInfo;
}

void CommuniCation::WriteData(const QByteArray& MessageData)
{
    Socket->write(MessageData);
    Socket->flush();
}

void CommuniCation::ClientDisconnected()
{
    emit Disconnected(Socket,this);
}

void CommuniCation::ModiProductInfo(const QBuffer &buffer)
{
    ProcessBuffer(buffer,MODI_PRODUCT);
}

void CommuniCation::AddProductInfo(const QBuffer &buffer)
{
    ProcessBuffer(buffer,ADD_PRODUCT);
}

void CommuniCation::DelProductInfo(const QBuffer &buffer)
{
    ProcessBuffer(buffer,DELETE_PRODUCT);
}

void CommuniCation::ConfrimLogin(const QBuffer &buffer)
{
    ProcessBuffer(buffer,CONFIRM_LOGIN);
}

void CommuniCation::Join(const QBuffer &buffer)
{
    ProcessBuffer(buffer,JOIN);
}

void CommuniCation::AddOrderInfo(const QBuffer &buffer)
{
    ProcessBuffer(buffer,ADD_ORDER);
}

void CommuniCation::ChattingParse(const QBuffer &buffer)
{
    ProcessBuffer(buffer,CHAT_MESG);
}

void CommuniCation::DuplicIdCheck(const QBuffer &buffer)
{
    ProcessBuffer(buffer,ID_CHECK);
}
