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
    void ClientConnect  ();// 클라이언트 연결시

    void DisConnectEvent(QTcpSocket* Socket,CommuniCation* Thread);// 클라이언트 종료시
    void BroadCast(QByteArray ChatData,QString chatRoomId, QSharedPointer<ClientInfo> UserInfo);// 채팅 메시지 받았을 때
    void SetCInfo(CommuniCation* Thread,ClientInfo* Info);// 클라이언트 정보 완성되서 넘길때
    void ProductModi(CommuniCation* Thread,const QByteArray& MessageData);// 상품정보 수정 요청 들어왔을때
    void LoadProductDB(CommuniCation* Thread);// 상품정보 조회 요청 들어왔을때
    void ProductAdd(CommuniCation* Thread,const QBuffer& MessageData);// 상품정보 추가 요청 들어왔을때
    void ProductDel(CommuniCation* Thread,const QBuffer& MessageData);// 상품정보 삭제 요청 들어왔을때
    void ConfirmLogin(CommuniCation* Thread,const QBuffer& MessageData);// 로그인 정보 확인 요청(확인 후 고객정보를 넘기거나 에러를 넘김)
    void Join(const QBuffer& MessageData);// 회원가입 요청
    void LoadUserInfo(CommuniCation* Thread);// 고객정보 요청
    void OrderAdd(CommuniCation* Thread,const QBuffer& MessageData);// 주문정보 추가
    void LoadOrderInfo(CommuniCation* Thread);// 주문정보 조회
    void LoadChatLogInfo(CommuniCation* Thread);// 채팅로그 조회
    void CheckId(CommuniCation* Thread,const QBuffer& MessageData);//아이디 중복조회
    void LoadThatOrderInfo(CommuniCation* Thread); //개인 고객 주문정보 조회
    void ChatLogAdd(const QBuffer &MessageData, QSharedPointer<ClientInfo> UserInfo);//채팅로그 저장

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

    const qint64 PD_ALL    = 0x03;
    const qint64 PD_MODI   = 0x04;
    const qint64 PD_ADD    = 0x05;
    const qint64 PD_DEL    = 0x06;
    const qint64 CONFIRM   = 0x07;
    const qint64 JOIN      = 0x08;
    const qint64 USER_ALL  = 0x09;
    const qint64 ORDER_ADD = 0x10;
    const qint64 ORDER_ALL = 0x11;
    const qint64 CHAT_ALL  = 0x12;
    const qint64 CHAT_MESG = 0x13;
    const qint64 ID_CHECK  = 0x14;
    const qint64 ORDER_LI  = 0x15;

    void SendData(const QByteArray &Data, CommuniCation *Thread, const qint64 &Comand);
};
#endif // WIDGET_H
