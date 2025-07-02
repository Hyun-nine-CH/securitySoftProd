#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

#include "parsing.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void ClientConnect();
    void BroadCast();

private:
    QLabel *InfoLabel;
    QLabel *PortLabel;
    QLabel *ChatLabel;
    QTcpServer *TcpServer;
    QVector<QTcpSocket*> ClientList;
    bool IsInfo; //정보 인지 채팅인지 확인하는 플래그

    Parsing *Parse;
};
#endif // WIDGET_H
