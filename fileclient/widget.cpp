#include "widget.h"
#include <QLabel>
#include <QTcpSocket>
#include <QFile>
#include <QFileDialog>
#include <QByteArray>
#include <QPushButton>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QJsonDocument>
#include <QJsonObject>

Widget::Widget(QWidget *parent)
    : QWidget(parent), isSent(false)
{
    infoLabel = new QLabel("Hello World",this);

    QPushButton *button = new QPushButton("Send",this);
    connect(button, SIGNAL(clicked()),SLOT(sendData()));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(infoLabel);
    layout->addWidget(button);

    tcpClient = new QTcpSocket(this);
    connect(tcpClient, SIGNAL(connected()),SLOT(send()));
    connect(tcpClient, SIGNAL(bytesWritten(qint64)),SLOT(goOnSend(qint64)));

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();
}

Widget::~Widget() {}

void Widget::send()
{
    //첨부파일 전송
    byteToWrite = totalSize = file->size();
    loadSize = 1024;

    QDataStream out(&outBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << qint64(0) << qint64(0) << qint64(0) << filename;

    totalSize += outBlock.size();
    byteToWrite += outBlock.size();

    out.device()->seek(0);
    qint64 dataType = 0x01;
    out << dataType << totalSize << qint64(outBlock.size());

    tcpClient->write(outBlock);

    progressDialog->setMaximum(totalSize);
    progressDialog->setValue(totalSize-byteToWrite);
    progressDialog->show();
}

void Widget::goOnSend(qint64 numBytes)
{
    byteToWrite -= numBytes;
    outBlock = file->read(qMin(byteToWrite,numBytes));
    tcpClient->write(outBlock);
    progressDialog->setMaximum(totalSize);
    progressDialog->setValue(totalSize - byteToWrite);

    if(byteToWrite == 0){
        infoLabel->setText(tr("File sending completed!"));
        progressDialog->reset();
    }
}

void Widget::openFile()
{
    loadSize = byteToWrite = totalSize = 0;
    outBlock.clear();

    filename = QFileDialog::getOpenFileName(this);
    file = new QFile(filename);
    file->open(QFile::ReadOnly);

    infoLabel->setText(tr("file %1 is opened").arg(filename));
    progressDialog->setValue(0);
}


void Widget::sendData()
{
    openFile();
    if(!filename.length()) return;
    if(!isSent){
        tcpClient->connectToHost(QHostAddress("127.0.0.1"),50000);
        isSent = true;
    }
    //send();

    infoLabel->setText(tr("Sending file %1").arg(filename));
}
