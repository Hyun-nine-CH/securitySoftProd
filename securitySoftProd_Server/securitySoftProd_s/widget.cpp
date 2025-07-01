#include <QtGui>
#include <QtNetwork>
#include <QtWidgets>
#include "widget.h"

#define BLOCK_SIZE 1024

//코드 할거
/*
    채팅
    1. 클라이언트 여러개 받는거
    2. 클라이언트에 브로드 캐스트 하는거

    파일
    1. 클라이언트에서 파일 받는거 QJson이용
    2. 클라이언트에서 받은 데이터(일단 이건 논의해서) 파싱 함수
*/

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    infoLabel = new QLabel(this);
    QPushButton *quitButton = new QPushButton("Quit",this);
    connect(quitButton,SIGNAL(clicked()),qApp,SLOT(quit()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    tcpServer = new QTcpServer(this);
    //클라이언트 감지
    connect(tcpServer, SIGNAL(newConnection()), SLOT(clientConnect()));
    if(!tcpServer->listen()){
        QMessageBox::critical(this,tr("Echo server"),\
                                                      tr("Unable to start the server: %1")\
                                                          .arg(tcpServer->errorString()));
        close();
        return;
    }
    infoLabel->setText(tr("the server is running on port %1")\
                           .arg(tcpServer->serverPort()));
    setWindowTitle(tr("Echo Server"));
}

void Widget::clientConnect()
{
    //클라이언트 정의
    //여기서 이제 QVector로 QTcpSocket을 만들어서 클라이언트들을 저장
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),\
            clientConnection, SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(readyRead()),SLOT(echoData()));
    infoLabel->setText("new connection is established...");
}

void Widget::echoData()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket*>(sender());
    if(clientConnection->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
    clientConnection->write(bytearray);
    infoLabel->setText(QString(bytearray));
}

Widget::~Widget() {}
