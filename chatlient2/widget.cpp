#include "widget.h"
#include <QtGui>
#include <QtNetwork>
#include <QtWidgets>
#include <QJsonDocument>

#define BLOCK_SIZE 1024

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QLineEdit *serverAddress = new QLineEdit(this);
    serverAddress->setText("127.0.0.1");
    serverAddress->setPlaceholderText("Server IP Address");
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|");
    QRegularExpressionValidator validator(re);
    serverAddress->setValidator(&validator);

    QLineEdit *serverPort = new QLineEdit(this);
    serverPort->setInputMask("00000;_");
    serverPort->setPlaceholderText("Server Port no");

    QPushButton *connectButton = new QPushButton("connect",this);
    connect(connectButton,&QPushButton::clicked,[=]{
        clientSocket->connectToHost(serverAddress->text(),serverPort->text().toInt());
        // 1. QJsonObject 생성
        QJsonObject jsonObject;
        jsonObject["type"] = "SendInfoData";  // 메시지 타입
        jsonObject["RoomId"] = "5";  // 메시지 내용
        jsonObject["ClientId"] = "Lee";  // 보내는 사람
        // 필요한 다른 필드들도 추가 가능

        // 2. QJsonObject를 QJsonDocument로 변환
        QJsonDocument jsonDoc(jsonObject);

        // 3. QJsonDocument를 QByteArray로 변환
        byteArray = jsonDoc.toJson();
        filename = "it is info";
        ContainData.clear();
        QDataStream out(&ContainData,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_15);
        out << qint64(0) << qint64(0) << qint64(0) << filename;
        ContainData.append(byteArray);
        totalSize += ContainData.size();

        out.device()->seek(0);
        qint64 dataType = 0x02;
        out << dataType << totalSize << totalSize;

        clientSocket->write(ContainData);
    });

    QPushButton *productButton = new QPushButton("Product",this);
    connect(productButton,&QPushButton::clicked,[=]{
        // 1. QJsonObject 생성
        QJsonObject jsonObject;
        jsonObject["type"] = "SendInfoData";  // 메시지 타입
        jsonObject["RoomId"] = "5";  // 메시지 내용
        jsonObject["ClientId"] = "Lee";  // 보내는 사람
        // 필요한 다른 필드들도 추가 가능

        // 2. QJsonObject를 QJsonDocument로 변환
        QJsonDocument jsonDoc(jsonObject);

        // 3. QJsonDocument를 QByteArray로 변환
        byteArray = jsonDoc.toJson();
        filename = "it is info";
        ContainData.clear();
        QDataStream out(&ContainData,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_15);
        out << qint64(0) << qint64(0) << qint64(0) << filename;
        ContainData.append(byteArray);
        totalSize += ContainData.size();

        out.device()->seek(0);
        qint64 dataType = 0x03;
        out << dataType << totalSize << totalSize;

        clientSocket->write(ContainData);
    });

    QPushButton *modiButton = new QPushButton("M",this);
    connect(modiButton,&QPushButton::clicked,[=]{
        // 1. QJsonObject 생성

        QJsonObject jsonObject;
        jsonObject["productId"] = "PD004";
        jsonObject["productName"] = "노이즈 캔슬링 헤드폰";
        jsonObject["price"] = 2500;
        jsonObject["stock"] = 1;
        jsonObject["description"] = "탁월한 노이즈 캔슬링 기능과 고음질 사운드를 자랑하는 프리미엄 헤드폰입니다. 긴 배터리 수명.";
        jsonObject["category"] = "Modify hi";
        jsonObject["manufacturer"] = "Soundscape Audio";
        jsonObject["imageUrl"] = "https://example.com/images/headphone004.jpg";

        // 2. QJsonObject를 QJsonDocument로 변환
        QJsonDocument jsonDoc(jsonObject);

        // 3. QJsonDocument를 QByteArray로 변환
        byteArray = jsonDoc.toJson();
        filename = "it is info";
        ContainData.clear();
        QDataStream out(&ContainData,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_15);
        out << qint64(0) << qint64(0) << qint64(0) << filename;
        ContainData.append(byteArray);
        totalSize += ContainData.size();

        out.device()->seek(0);
        qint64 dataType = 0x04;
        out << dataType << totalSize << totalSize;

        clientSocket->write(ContainData);
    });

    QHBoxLayout *serverLayout = new QHBoxLayout;
    serverLayout->addStretch(1);
    serverLayout->addWidget(serverAddress);
    serverLayout->addWidget(serverPort);
    serverLayout->addWidget(connectButton);

    message = new QTextEdit(this);
    message->setReadOnly(true);

    inputLine = new QLineEdit(this);
    QPushButton *sentButton = new QPushButton("Send",this);
    connect(sentButton, SIGNAL(clicked()),SLOT(sendData()));

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputLine);
    inputLayout->addWidget(sentButton);

    QPushButton *quitButton = new QPushButton("Quit",this);
    connect(quitButton, SIGNAL(clicked()),qApp,SLOT(quit()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    serverLayout->addStretch(1);
    inputLayout->addWidget(quitButton);
    inputLayout->addWidget(productButton);
    inputLayout->addWidget(modiButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(serverLayout);
    mainLayout->addWidget(message);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    clientSocket = new QTcpSocket(this);
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{qDebug() << clientSocket->errorString();});
    connect(clientSocket, SIGNAL(readyRead()),SLOT(echoData()));
    connect(clientSocket, SIGNAL(bytesWritten(qint64)),SLOT(goOnSend(qint64)));
    setWindowTitle(tr("Echo Client"));
}

void Widget::goOnSend(qint64 numBytes)
{
    if (numBytes <= 0) {
        qDebug() << "전송된 바이트 없음 또는 에러 발생: " << numBytes;
        return; // 아무것도 전송되지 않았으면 그냥 리턴
    }
    // byteToWrite -= numBytes;
    // if (byteToWrite > 0) {
    //     // 이미 보낸 부분을 제외하고 남은 데이터만 전송
    //     clientSocket->write(ContainData.right(byteToWrite));
    // }
}

Widget::~Widget()
{
    clientSocket->close();
}

void Widget::echoData()
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket*>(sender());
    //if(clientSocket->bytesAvailable() > BLOCK_SIZE) return;
    //QByteArray byte = clientSocket->read(BLOCK_SIZE);
    QByteArray byte = clientSocket->readAll();
    message->append(QString(byte));
}

void Widget::sendData()
{

    QString str = inputLine->text();
    if(str.length()){
        QByteArray byte;
        byte = str.toUtf8();
        clientSocket->write(byte);
    }
}


