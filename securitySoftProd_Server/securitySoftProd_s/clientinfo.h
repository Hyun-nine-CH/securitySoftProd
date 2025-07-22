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
    QByteArray  getClientData  () const;
    bool        getIsInvite    () const;
    void        setClientData  (const QByteArray &Data);
    void        setClientID    (const int        &Id);
    void        setClientRoomId(const QString    &RoomId);
    void        setClientNick  (const QString    &nick);
    void        setIsInvite    (const bool       invite);

    void ChangeJsonData(); //처음에 받은 데이터를 JSON으로 변환

private:
    int        CId;       //클라이언트 식별 ID
    QString    RId;       //클라이언트 소속 채팅방
    QString    ID;        //클라이언트 닉네임 ID
    QByteArray InfoData;
    bool       isInvite = false; //초대되었나
};

#endif // CLIENTINFO_H
