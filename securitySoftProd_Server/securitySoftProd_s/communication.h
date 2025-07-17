#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QFile>
#include <QBuffer>
#include <QMutex>
#include <QSharedPointer>

#include "clientinfo.h"
#include "productdb.h"

class CommuniCation : public QThread
{
    Q_OBJECT
public:
    CommuniCation(QTcpSocket* socket, ClientInfo* myInfo,QObject* parent = nullptr);
    QSharedPointer<ClientInfo> getClientInfo();

protected:
    void run() override; // 스레드에서 실행될 주요 로직

signals:
    void Disconnected       (QTcpSocket* Socket,CommuniCation* Thread);// 스레드에서 소켓 끊김을 알리는 시그널
    void ChattingMesg       (const QBuffer& MessageData, QSharedPointer<ClientInfo> UserInfo);// 채팅 메시지 시그널 + 채팅로그 저장
    void SendClientInfo     (CommuniCation* Thread,ClientInfo *Info);// 클라이언트 정보 넘기는 시그널
    void ModifyProductDB    (CommuniCation* Thread,const QByteArray& MessageData);// 상품데이터 수정 정보 넘기는 시그널
    void RequestPdInfo      (CommuniCation* Thread);// 상품데이터 전체 정보 넘기는 시그널
    void RequestPdAdd       (CommuniCation* Thread,const QBuffer& MessageData);// 상품데이터 추가 요청
    void RequestPdDel       (CommuniCation* Thread,const QBuffer& MessageData); // 상품데이터 삭제 요청
    void RequestConfirm     (CommuniCation* Thread,const QBuffer& MessageData);// 로그인 확인 후 정보 보내기
    void RequestJoin        (const QBuffer& MessageData);// 회원가입 요청
    void RequestUserInfo    (CommuniCation* Thread);// 고객정보 요청
    void RequestOrderAdd    (CommuniCation* Thread,const QBuffer& MessageData);// 주문정보 추가
    void RequestOrderInfo   (CommuniCation* Thread);// 주문정보 조회
    void RequestChatLogInfo (CommuniCation* Thread);// 채팅로그 조회
    void RequestIdCheck     (CommuniCation* Thread,const QBuffer& MessageData); //아이디 중복체크
    void RequestThatOrder   (CommuniCation* Thread); //그 고객에 대한 주문정보 조회

private slots:
    void ReadClientData    (); // 소켓에서 데이터 읽기
    void ClientDisconnected(); // 소켓 연결 끊김 처리
    void WriteData         (const QByteArray& MessageData);

private:
    QFile      *NewFile;
    QByteArray ByteArray;
    QString    FileName;
    qint64     TotalSize;
    qint64     CurrentPacket;
    qint64     DataType;
    qint64     ReceivePacket;
    QTcpSocket *Socket;

    QSharedPointer<ClientInfo> CInfo;
    QThread    WorkThread;
    ProductDB  *PdDb;

    enum RequestType {
        MODI_PRODUCT = 1,
        ADD_PRODUCT,
        DELETE_PRODUCT,
        CONFIRM_LOGIN,
        JOIN,
        ADD_ORDER,
        ADD_LOG,
        CHAT_MESG,
        ID_CHECK
    };

    void ProcessBuffer        (const QBuffer &buffer, int requestType);
    void FileReceive          (const QBuffer &buffer);//첨부파일
    void ChatMessageReceive   (const QBuffer &buffer);//채팅 정보
    void ModiProductInfo      (const QBuffer &buffer);//상품 수정
    void AddProductInfo       (const QBuffer &buffer);//상품 추가
    void DelProductInfo       (const QBuffer &buffer);//상품 삭제
    void ConfrimLogin         (const QBuffer &buffer);//로그인 정보 확인
    void Join                 (const QBuffer &buffer);//회원정보 추가
    void AddOrderInfo         (const QBuffer &buffer);//주문정보 추가
    void ChattingParse        (const QBuffer &buffer);//채팅 메시지 파싱
    void DuplicIdCheck        (const QBuffer &buffer);//아이디 중복
};

#endif // COMMUNICATION_H
