#include <QtGui>
#include <QtNetwork>
#include <QtWidgets>

#include "widget.h"

#define BLOCK_SIZE 1024

//코드 할거
/*
    채팅
    1. 클라이언트 여러개 받는거 (o)
    2. 클라이언트에 브로드 캐스트 하는거 (o)

    파일
    1. 클라이언트에서 파일 받는거 QJson이용
    2. 클라이언트에서 받은 데이터(일단 이건 논의해서) 파싱 함수
*/

Widget::Widget(QWidget *parent)
    : QWidget(parent), IsInfo(false)
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
    if(!TcpServer->listen()){
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
    ClientList.append(ClientConnection);
    //소켓에 대한 읽고 닫기 연결
    connect(ClientConnection, SIGNAL(disconnected()),\
            ClientConnection, SLOT(deleteLater()));
    connect(ClientConnection, SIGNAL(readyRead()),SLOT(BroadCast()));
    //클라이언트 몇개가 연결되었는지 확인
    int ClientNum = ClientList.size();
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
    if(Parse->IsInfo(ByteArray)){
        IsInfo = true;
    }
    if(!IsInfo){
        //브로드캐스팅
        for(QVector<QTcpSocket*>::const_iterator it = ClientList.constBegin();\
            it != ClientList.constEnd(); ++it){
            QTcpSocket *Socket = *it; // 이터레이터가 가리키는 실제 값(QTcpSocket* 포인터)을 가져옴
            Socket->write(ByteArray);
        }
    }

    ChatLabel->setText(QString(ByteArray));
}

Widget::~Widget() {}
