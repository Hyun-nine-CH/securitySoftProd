#ifndef CLIENTINFO_H
#define CLIENTINFO_H
#include <QByteArray>
#include <QTcpSocket>

class ClientInfo
{
public:
    ClientInfo();

    int         getClientID    () const;
    QString     getClientRoomId() const;
    QString     getClientNick  () const;
    //QTcpSocket* getClientSocket() const;
    QByteArray  getClientData  () const;
    void        setClientData  (const QByteArray &Data);
    void        setClientID    (const int    &Id);
    void        setClientRoomId(const QString    &RoomId);
    //void        setClientSocket(      QTcpSocket *soc);
    void        setClientNick  (const QString    &nick);

    void ChangeJsonData(); //처음에 받은 데이터를 JSON으로 변환

private:
    int        CId;       //클라이언트 식별 ID
    QString    RId;       //클라이언트 소속 채팅방
    QString    ID;        //클라이언트 닉네임 ID
    //QTcpSocket *socket;   //클라이언트 소캣
    QByteArray InfoData;
};

#endif // CLIENTINFO_H
