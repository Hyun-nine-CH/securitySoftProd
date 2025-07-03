#include <QtGui>
#include <QtNetwork>
#include <QtWidgets>
#include <QDataStream>
#include "widget.h"

#define BLOCK_SIZE 1024

//코드 한거거
/*
    20250702
    <파일이 아닌 QByteArray에 대한>
    브로드캐스트(채팅방 구분하여)
    클라이언트 정보 클래스 구현
    고정 포트 설정
    QMap으로 클라이언트 정보 저장 (클라이언트 정보클래스 이용해서)
    <첨부파일 기능 구현 중>

    20250703
    헤더 추가하여 채팅인지 파일인지 구분하는 기능 추가
        - 프로토콜 QStreamData로 할것
*/

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    //지워야 함
    pdb = new ProductDB();
    InfoLabel     = new QLabel(this);
    PortLabel     = new QLabel(this);
    ChatLabel     = new QLabel(this);
    TotalSize     = 0;
    CurrentPacket = 0;
    DataType      = 0;
    ReceivePacket = 0;

    QPushButton *QuitButton = new QPushButton("Quit",this);
    connect(QuitButton,SIGNAL(clicked()),qApp,SLOT(quit()));

    QHBoxLayout *ButtonLayout = new QHBoxLayout;
    ButtonLayout->addStretch(1);
    ButtonLayout->addWidget(QuitButton);

    QVBoxLayout *MainLayout = new QVBoxLayout(this);
    MainLayout->addWidget(InfoLabel);
    MainLayout->addWidget(PortLabel);
    MainLayout->addWidget(ChatLabel);
    MainLayout->addLayout(ButtonLayout);
    setLayout(MainLayout);

    TcpServer = new QTcpServer(this);
    //클라이언트 감지
    connect(TcpServer, SIGNAL(newConnection()), SLOT(ClientConnect()));
    if(!TcpServer->listen(QHostAddress::Any, 50000)){
        QMessageBox::critical(this,tr("Echo server"),\
                  tr("Unable to start the server: %1")\
                      .arg(TcpServer->errorString()));
        close();
        return;
    }
    PortLabel->setText(tr("the server is running on port %1")\
                           .arg(TcpServer->serverPort()));
    setWindowTitle(tr("Server"));
    //test
    pdb->LoadProductData();
}

void Widget::ClientConnect()
{
    //클라이언트 연결
    QTcpSocket *ClientConnection = TcpServer->nextPendingConnection();
    //여기서 이제 QVector로 QTcpSocket을 만들어서 클라이언트들을 저장
    CInfo = new ClientInfo();
    CInfo->setClientSocket(ClientConnection);
    CInfoList.insert(ClientConnection,CInfo);
    //소켓에 대한 읽고 닫기 연결
    connect(ClientConnection, SIGNAL(disconnected()),\
            this, SLOT(DisConnectEvent()));
    connect(ClientConnection, SIGNAL(readyRead()),SLOT(BroadCast()));
    //클라이언트 몇개가 연결되었는지 확인
    int ClientNum = CInfoList.size();
    InfoLabel->setText(tr("%1 connection is established...").arg(ClientNum));
}

void Widget::BroadCast()
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
    ClientInfo* ThatClient = CInfoList.value(ClientConnection);
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
    case 0x01:FileReceive(buffer);break;
    case 0x02:ClientInitDataReceive(buffer);break;
    case 0x03:LoadProductDB(); break;
    default:
        if(ThatClient){
            for(QMap<QTcpSocket*, ClientInfo*>::const_iterator it = CInfoList.constBegin();\
                                                                                              it != CInfoList.constEnd(); ++it){
                ClientInfo *C = it.value(); // 이터레이터가 가리키는 실제 값(QTcpSocket* 포인터)을 가져옴
                //같은 방이면 브로드캐스트 해라
                qDebug() << "compare : " << C->getClientRoomId();
                qDebug() << "origin  : " << ThatClient->getClientRoomId();
                if(QString::compare(C->getClientRoomId(), ThatClient->getClientRoomId()) == 0)
                {
                    C->getClientSocket()->write(ByteArray);
                    C->getClientSocket()->flush();
                }
            }
        }
        break;
    }

    ChatLabel->setText(QString(ByteArray));
    ByteArray.clear();
}

void Widget::FileReceive(const QBuffer &buffer)
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
        InfoLabel->setText(tr("%1 receive completed").arg(FileName));
        ByteArray.clear();
        ReceivePacket = 0;
        TotalSize = 0;
        DataType = 0;
        NewFile->close();
        delete NewFile;
        NewFile = nullptr;
    }
}

void Widget::ClientInitDataReceive(const QBuffer &buffer)
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
        InfoLabel->setText(tr("client info receive completed"));
        CInfo->setClientData(ByteArray);
        qDebug() << "end 내용 : " << ByteArray;
        CInfo->ChangeJsonData();
        ReceivePacket = 0;
        TotalSize = 0;
        DataType = 0;
        ByteArray.clear();
    }
}

void Widget::LoadProductDB()
{

}

void Widget::ChatMessageReceive(const QBuffer &buffer)
{

}

void Widget::DisConnectEvent()
{
    QTcpSocket* DisConnectSocket = qobject_cast<QTcpSocket*>(sender());
    if (DisConnectSocket) {
        // QMap에서 ClientInfo*를 빠르게 찾음
        if (CInfoList.contains(DisConnectSocket)) {
            /*
                Info의 의미
                CInfoList에서 DisConnectSocket를 키로 사용하여
                해당 키에 연결된 값(value), 즉 ClientInfo* 포인터를 가져오는 역할
            */
            ClientInfo* Info = CInfoList.value(DisConnectSocket);

            /*
             *  romove의 의미
                CInfoList에서 키-값 쌍 자체를 제거. 즉, DisConnectSocket이라는 키에
                연결된 ClientInfo* (값)의 관계를 끊음. 그러나!!
                값으로 저장되어 있던 ClientInfo* 포인터가 가리키는
                실제 ClientInfo 객체의 메모리까지 해제해주지는 않음
            */
            CInfoList.remove(DisConnectSocket);

            // 가져온 ClientInfo* 포인터를 사용하여 실제 ClientInfo 객체의 메모리를 해제
            delete Info;
            //유효하지 않은 메모리 접근 방지. delete를 썼기때문에 nullptr해준것
            Info = nullptr;

            qDebug() << "QMap에서 클라이언트 제거 완료.";
        }

        // 소켓 삭제 예약
        DisConnectSocket->deleteLater();
    }
    // UI 업데이트: CInfoList.size() 사용
    InfoLabel->setText(tr("%1 connection is established...").arg(CInfoList.size()));
}

Widget::~Widget()
{

}
