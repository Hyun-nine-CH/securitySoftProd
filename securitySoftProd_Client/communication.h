#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QBuffer>

class Communication : public QObject
{
    Q_OBJECT
public:
    static Communication* getInstance(QObject *parent = nullptr);
    ~Communication();

    QJsonObject getUserInfo() const;
    QTcpSocket* getSocket();

    Communication(const Communication&) = delete;
    Communication& operator=(const Communication&) = delete;

private:
    explicit Communication(QObject *parent = nullptr);
    QTcpSocket* socket;
    QByteArray  m_buffer;
    QJsonObject m_userInfo;
    qint64      TotalSize;
    qint64      CurrentPacket;
    qint64      DataType;
    qint64      ReceivePacket;

    enum RequestType {
        LOGIN = 1,

    };

    void ProcessBuffer(const QBuffer &buffer, int requestType);
    void setUserInfo(const QByteArray &buffer);

    static Communication* instance;

public slots:
    void SendLoginConfirm(const QString& id, const QString& pw);


private slots:
    void Login(const QBuffer &buffer); //로그인
    void StartComm();
    //void SignUp(); //
    void onReadyRead();


signals:
    void LoginSuccess();
    void LoginFail();
    //void ReceiveUserInfo();
    //void SendAddUser();


};

#endif // COMMUNICATION_H
