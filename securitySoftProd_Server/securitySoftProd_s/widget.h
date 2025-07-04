#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>
#include <QMap>
#include <QFile>
#include <QBuffer>
#include <QThread>

#include "clientinfo.h"
#include "communication.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    // 클라이언트 연결시
    void ClientConnect  ();
    // 클라이언트 종료시
    void DisConnectEvent(      QTcpSocket*    Socket,
                               CommuniCation* Thread);
    // 채팅 메시지 받았을 때
    void BroadCast      (const QByteArray&    MessageData,
                         const QString&       RoomId);
    // 클라이언트 정보 완성되서 넘길때
    void SetCInfo       (      CommuniCation* Thread,
                               ClientInfo*    Info);
    // 상품정보 수정 요청 들어왔을때
    void ProductModi    (const QByteArray&    MessageData);

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
    QMutex     *ListMutex;

    ClientInfo    *CInfo;
    CommuniCation *Comm;

    QMap<CommuniCation*, ClientInfo*> CInfoList;

    void LoadProductDB();
};
#endif // WIDGET_H
