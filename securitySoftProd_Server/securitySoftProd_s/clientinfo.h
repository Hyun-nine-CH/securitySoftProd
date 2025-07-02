#ifndef CLIENTINFO_H
#define CLIENTINFO_H
#include <QByteArray>
#include <QTcpSocket>

class ClientInfo
{
public:
    ClientInfo();

    QString     getClientID    ();
    void        setClientID    (const QString &Id);
    QString     getClientRoomId();
    void        setClientRoomId(const QString &RoomId);
    QTcpSocket* getClientSocket();
    void        setClientSocket(QTcpSocket *soc);
    QByteArray  getClientData();
    void        setClientData(const QByteArray &Data);

    void ChangeJsonData(); //처음에 받은 데이터를 JSON으로 변환

private:
    QString    CId;       //클라이언트 고객 ID
    QString    RId;       //클라이언트 소속 채팅방
    QTcpSocket *socket;   //클라이언트 소캣
    QByteArray InfoData;
};

#endif // CLIENTINFO_H
