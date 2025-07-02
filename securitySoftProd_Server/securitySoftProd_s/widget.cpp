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

*/

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    InfoLabel = new QLabel(this);
    PortLabel = new QLabel(this);
    ChatLabel = new QLabel(this);
    Parse     = new Parsing();

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
    if(ClientConnection->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray ByteArray = ClientConnection->read(BLOCK_SIZE);
    ClientInfo* ThatClient = CInfoList.value(ClientConnection);

    //파일을 받았을때
    QByteArray PeekedData = ClientConnection->peek(16);
    if(Parse->IsInfo(ByteArray) == false){
        //브로드캐스팅
        if(Parse->IsFile(PeekedData)){
            if(ByteReceived == 0){
                QDataStream In(ClientConnection);
                In >> TotalSize >> ByteReceived >> FileName;
                QFileInfo info(FileName);
                QString CurrentFileName = info.fileName();
                NewFile = new QFile(CurrentFileName);
                NewFile->open(QFile::WriteOnly);
            }else{
                InBlock = ClientConnection->readAll();
                ByteReceived += InBlock.size();
                NewFile->write(InBlock);
                NewFile->flush();
            }
            if(ByteReceived == TotalSize){
                InfoLabel->setText(tr("%1 receive completed").arg(FileName));
                InBlock.clear();
                ByteReceived = 0;
                TotalSize = 0;
                NewFile->close();
                delete NewFile;
                NewFile = nullptr;
            }
        } else{
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
        }
    }else{
        CInfo->setClientData(ByteArray);
        CInfo->ChangeJsonData();
    }



    ChatLabel->setText(QString(ByteArray));
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
