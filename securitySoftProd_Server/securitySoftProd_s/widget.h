#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>
#include <QMap>
#include <QFile>

#include "parsing.h"
#include "clientinfo.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void ClientConnect();
    void BroadCast();
    void DisConnectEvent();

private:
    QLabel     *InfoLabel;
    QLabel     *PortLabel;
    QLabel     *ChatLabel;
    QTcpServer *TcpServer;
    QFile      *NewFile;
    QByteArray InBlock;
    QString    FileName;
    qint64     TotalSize;
    qint64     ByteReceived;
    QMap<QTcpSocket*, ClientInfo*> CInfoList;

    Parsing *Parse;
    ClientInfo *CInfo;
};
#endif // WIDGET_H
