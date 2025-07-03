#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QFile>
#include <QBuffer>
#include <QMutex>

#include "clientinfo.h"

class CommuniCation : public QThread
{
    Q_OBJECT
public:
    CommuniCation(QTcpSocket* socket, ClientInfo* myInfo, \
                  QMutex* mapMutex,QObject* parent = nullptr);
    ClientInfo* getClientInfo();

protected:
    void run() override; // 스레드에서 실행될 주요 로직

signals:
    // 스레드에서 소켓 끊김을 알리는 시그널
    void Disconnected(QTcpSocket* Socket);
    // 채팅 메시지 시그널
    void ChattingMesg(const QByteArray& MessageData, const QString& RoomId);
private slots:
    void ReadClientData(); // 소켓에서 데이터 읽기
    void ClientDisconnected(); // 소켓 연결 끊김 처리
    void ClientConnect();
    void WriteData(const QByteArray& MessageData);

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

    void FileReceive(const QBuffer &buffer);
    void ClientInitDataReceive(const QBuffer &buffer);
    void ChatMessageReceive(const QBuffer &buffer);

};

#endif // COMMUNICATION_H
