#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QFile>
#include <QBuffer>
#include <QMutex>

#include "clientinfo.h"
#include "productdb.h"

class CommuniCation : public QThread
{
    Q_OBJECT
public:
    CommuniCation(QTcpSocket* socket, ClientInfo* myInfo,QObject* parent = nullptr);
    ClientInfo* getClientInfo();

protected:
    void run() override; // 스레드에서 실행될 주요 로직

signals:
    // 스레드에서 소켓 끊김을 알리는 시그널
    void Disconnected   (      QTcpSocket*    Socket,
                               CommuniCation* Thread);
    // 채팅 메시지 시그널
    void ChattingMesg   (const QByteArray&    MessageData,
                         const QString&       RoomId);
    // 클라이언트 정보 넘기는 시그널
    void SendClientInfo (      CommuniCation* Thread,
                               ClientInfo     *Info);
    // 상품데이터 수정 정보 넘기는 시그널
    void ModifyProductDB(const QByteArray&    MessageData);
    // 상품데이터 전체 정보 넘기는 시그널
    void RequestPdInfo  (       CommuniCation* Thread);
    // 상품데이터 추가 요청
    void RequestPdAdd   (       CommuniCation* Thread,
                         const   QBuffer&       MessageData);

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

    ClientInfo *CInfo;
    QThread    WorkThread;
    ProductDB  *PdDb;

    //첨부파일
    void FileReceive          (const QBuffer &buffer);
    //클라이언트 초기 정보
    void ClientInitDataReceive(const QBuffer &buffer);
    //채팅 정보
    void ChatMessageReceive   (const QBuffer &buffer);
    //상품 정보 전체 조회
    void SendProductInfo      ();
    //상품 수정
    void ModiProductInfo      (const QBuffer &buffer);
    //상품 추가
    void AddProductInfo(const QBuffer &buffer);
};

#endif // COMMUNICATION_H
