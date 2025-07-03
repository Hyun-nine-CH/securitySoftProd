#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>
#include <QMap>
#include <QFile>

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
    QByteArray ByteArray;
    QString    FileName;
    qint64     TotalSize;
    qint64     CurrentPacket;
    qint64     DataType;
    qint64     ReceivePacket;
    QMap<QTcpSocket*, ClientInfo*> CInfoList;

    ClientInfo *CInfo;
};
#endif // WIDGET_H
