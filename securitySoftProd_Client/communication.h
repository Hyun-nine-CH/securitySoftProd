#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QBuffer>
#include <QFile>
class Communication : public QObject
{
    Q_OBJECT
public:
    static Communication* getInstance();
    ~Communication();

    QJsonObject getUserInfo() const;
    QTcpSocket* getSocket();

    Communication(const Communication&) = delete;
    Communication& operator=(const Communication&) = delete;


    void SendChatMesg(const QString &mesg); //채팅메시지 신호
    void SendChatMesg(const QByteArray &mesg);
    void SendIdCheck(const QByteArray &idcheck); //아이디 중복 서버에 체크해달라고
    void SendJoinData(const QByteArray &idcheck);//회원가입 데이터
    void SendOrderData(const QByteArray &data);
    void SendFile(QFile* file);
    void RequestActiveUserList();
private:
    explicit Communication();
    QTcpSocket* socket;
    QByteArray  m_buffer;
    QJsonObject m_userInfo;
    qint64      TotalSize;
    qint64      CurrentPacket;
    qint64      DataType;
    qint64      ReceivePacket;
    qint64      loadSize;
    qint64      byteToWrite;
    QByteArray  outBlock;
    bool        isFirst = true;
    bool        isRoomInit = true;

    enum RequestType {
        LOGIN = 1,
        ID_CHECK,
    };

    void ProcessBuffer(const QBuffer &buffer, int requestType);
    void setUserInfo(const QByteArray &buffer);

    static Communication* instance;
    void Receive_Product(const QBuffer &buffer);
    void Receive_UserInfo(const QBuffer &buffer);
    QFile* Files_;
    bool isFileSending = false;

public slots:
    void SendLoginConfirm(const QString& id, const QString& pw);
    void RequestProductInfo(); //제품목록 요청 신호
    void RequestProductInfo_ad(); //제품목록 요청 신호
    void RequestOrderInfo(); //주문조회 요청 신호
    void RequestAllOrderInfo(); // 모든 주문조회 요청 신호
    void RequestUserInfo(); //고객목록 요청 신호
    void RequestProductAdd(const QJsonObject& productData); //제품추가 요청 신호
    void RequestProductDel(const QJsonObject& productData); //제품삭제 요청 신호
    void RequestProductMod(const QJsonObject& productData); //제품수정 요청 신호
    void RequestInviteUser(QString UserId); //채팅 초대
    void RequestInviteExit();

private slots:
    void Login(const QBuffer &buffer); //로그인
    void IdChekc(const QBuffer &buffer); //아이디중복체크에 대한 결과값
    void StartComm();
    //void SignUp(); //
    void onReadyRead();
    void goOnSend(qint64);


signals:
    void LoginSuccess();
    void LoginFail();
    void ReceiveProductInfo(const QBuffer &buffer);
    void ReceiveProductInfo_ad(const QBuffer &buffer);
    void ReceiveUserInfo(const QBuffer &buffer);
    void ReceiveOrderInfo(const QBuffer &buffer);
    void ReceiveAllOrderInfo(const QBuffer &buffer);
    void ReceiveChat(const QBuffer &buffer);
    void IdCheckResult(bool IsDupli);
    void FinishInit();
    void ReceviceChatRoomInfo(const QBuffer &buffer);
    void ReceiveActiveUserList(const QBuffer &buffer);
    void InviteFromCorp();

};

#endif // COMMUNICATION_H
