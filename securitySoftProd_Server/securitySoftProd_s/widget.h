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
#include "datamanager.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void SendProductInfo(const QByteArray&    PInfo);

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
    void ProductModi    (      CommuniCation* Thread,
                         const QByteArray&    MessageData);
    // 상품정보 조회 요청 들어왔을때
    void LoadProductDB  (      CommuniCation* Thread);
    // 상품정보 추가 요청 들어왔을때
    void ProductAdd     (      CommuniCation* Thread,
                         const       QBuffer& MessageData);
    // 상품정보 삭제 요청 들어왔을때
    void ProductDel     (      CommuniCation* Thread,
                         const       QBuffer& MessageData);

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
    DataManager   *DMan;

    QMap<CommuniCation*, ClientInfo*> CInfoList;

    const qint64 PD_ALL  = 0x03;
    const qint64 PD_MODI = 0x04;
    const qint64 PD_ADD  = 0x05;
    const qint64 PD_DEL  = 0x06;

    void SendData(const QByteArray &Data, CommuniCation *Thread, const qint64 &Comand);


};
#endif // WIDGET_H
