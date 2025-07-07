#include <QtGui>
#include <QtNetwork>
#include <QtWidgets>
#include <QDataStream>
#include <QJsonDocument>

#include "widget.h"

//코드 한거
/*
    20250702
    <파일이 아닌 QByteArray에 대한>
    브로드캐스트(채팅방 구분하여)
    클라이언트 정보 클래스 구현
    고정 포트 설정
    QMap으로 클라이언트 정보 저장 (클라이언트 정보클래스 이용해서)
    <첨부파일 기능 구현 중>

    20250703
    헤더 추가하여 채팅인지 파일인지 구분하는 기능 추가
        - 프로토콜 QStreamData로 할것
    구조 갈아 엎음. QThread 따로 뺌

    20250704
    QThread 클래스 안정화
    데이터베이스 코드 작업중
    데이터베이스 부모 코드에 데이터매니저 추가함(초기화 방식이 특이하니 주의할것)
    product 추가 조회 기능 완성
    깃 이슈 사용

    20250706
    product modify 완성
    나중에 product에서 sendData 삭제할것
    product delete 작업 완료

    20250707
    클라이언트 로그인 프로토콜 맞춤
    클라이언트 로그인 confirm 작업 완료
    ClientInfo에서 roomid 변경 완료, 시점 변경완료(0x02 안지움)최종적으로 지울것
    클라이언트 DB 추가 기능 작업 가완료(테스트 안해봄)
    클라이언트 데이터 보내기 가완료 (테스트 안해봄)
    주문정보 추가 기능 작업중
*/

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    InfoLabel     = new QLabel(this);
    PortLabel     = new QLabel(this);
    ChatLabel     = new QLabel(this);
    ListMutex     = new QMutex();
    DMan          = new DataManager();
    TotalSize     = 0;
    CurrentPacket = 0;
    DataType      = 0;
    ReceivePacket = 0;

    QPushButton *QuitButton = new QPushButton("Quit",this);
    connect(QuitButton,SIGNAL(clicked()),qApp,SLOT(quit()));

    QHBoxLayout *ButtonLayout = new QHBoxLayout;
    ButtonLayout->addStretch(1);
    ButtonLayout->addWidget(QuitButton);

    QVBoxLayout *MainLayout = new QVBoxLayout(this);
    MainLayout->addWidget(InfoLabel);
    MainLayout->addWidget(PortLabel);
    MainLayout->addWidget(ChatLabel);
    MainLayout->addLayout(ButtonLayout);
    setLayout(MainLayout);

    TcpServer = new QTcpServer(this);

    if(!TcpServer->listen(QHostAddress::Any, 50000)){
        QMessageBox::critical(this,tr("Echo server"),\
                  tr("Unable to start the server: %1")\
                      .arg(TcpServer->errorString()));
        close();
        return;
    }
    PortLabel->setText(tr("the server is running on port %1")\
                           .arg(TcpServer->serverPort()));
    setWindowTitle(tr("Server"));
    //클라이언트 감지
    connect(TcpServer, SIGNAL(newConnection()), SLOT(ClientConnect()));
}

void Widget::ClientConnect()
{
    //클라이언트 연결
    QTcpSocket *ClientConnection = TcpServer->nextPendingConnection();
    //여기서 이제 QVector로 QTcpSocket을 만들어서 클라이언트들을 저장
    CInfo = new ClientInfo();
    Comm = new CommuniCation(ClientConnection,CInfo);
    ListMutex->lock();
    CInfoList.insert(Comm,CInfo);
    ListMutex->unlock();
    Comm->start(); // workerThread의 run() 메서드가 실행됩니다.
    connect(Comm, &QThread::finished, Comm, &QObject::deleteLater);// 스레드 종료 시 해당 스레드 객체를 자동으로 삭제하도록 연결
    connect(Comm, &CommuniCation::Disconnected, this, &Widget::DisConnectEvent);// 스레드에서 클라이언트 연결이 끊겼음을 알리는 시그널 연결 (서버에서 관리 용이)
    connect(Comm, &CommuniCation::ChattingMesg, this, &Widget::BroadCast);//브로드캐스트 시점 신호
    connect(Comm, &CommuniCation::SendClientInfo, this, &Widget::SetCInfo);//클라이언트 정보 받아서 할당
    connect(Comm, &CommuniCation::ModifyProductDB, this, &Widget::ProductModi);//상품정보 수정 데이터매니저에게요청
    connect(Comm, &CommuniCation::RequestPdInfo, this, &Widget::LoadProductDB);//상품정보 조회 요청
    connect(Comm, &CommuniCation::RequestPdAdd, this, &Widget::ProductAdd);//상품정보 추가 요청
    connect(Comm, &CommuniCation::RequestPdDel, this, &Widget::ProductDel);//상품정보 삭제 요청
    connect(Comm, &CommuniCation::RequestConfirm, this, &Widget::ConfirmLogin);//로그인 정보 확인 요청
    connect(Comm, &CommuniCation::RequestJoin, this, &Widget::Join);//회원가입 요청
    connect(Comm, &CommuniCation::RequestUserInfo, this, &Widget::LoadUserInfo);//고객정보 조회
    connect(Comm, &CommuniCation::RequestOrderAdd, this, &Widget::OrderAdd);//주문정보 추가
    connect(Comm, &CommuniCation::RequestOrderInfo, this, &Widget::LoadOrderInfo);//주문정보 추가
}

void Widget::BroadCast(const QByteArray& MessageData, const QString& RoomId)
{
    /*
        MessageData를 복사하여 전달
        그대로 받게 되면 다른 클라이언트에서 이 코드를 실행할때
        MessageData에 접근해서 크래시 나거나 데이터 오염됨
    */
    QByteArray messageCopy = MessageData;
    ListMutex->lock();
    for(QMap<CommuniCation*, ClientInfo*>::const_iterator it = CInfoList.constBegin();\
        it != CInfoList.constEnd(); ++it){
        ClientInfo *C = it.value(); // 이터레이터가 가리키는 실제 값(ClientInfo* 포인터)을 가져옴
        CommuniCation* W = it.key();
        //같은 방이면 브로드캐스트 해라
        qDebug() << "compare : " << C->getClientRoomId();
        qDebug() << "origin  : " << RoomId;
        if(QString::compare(C->getClientRoomId(), RoomId) == 0)
        {
            if(W)
            {
                // QMetaObject::invokeMethod를 사용하여 대상 스레드의 슬롯 호출
                // Qt::QueuedConnection: 호출이 대상 스레드의 이벤트 큐에 추가되고,
                // 대상 스레드의 이벤트 루프에서 안전하게 실행됩니다.
                QMetaObject::invokeMethod(W,"WriteData", // 호출할 슬롯 이름 (문자열)
                                          Qt::QueuedConnection,  // 연결 타입 (필수)
                                          Q_ARG(QByteArray, messageCopy)); // 슬롯에 전달할 인자
                qDebug() << "메시지 전송 요청됨: " << W->metaObject()->className();
            }
        }
    }
    ListMutex->unlock();
}

void Widget::SetCInfo(CommuniCation* Thread, ClientInfo *Info)
{
    if (Thread) {
        ListMutex->lock();
        if (CInfoList.contains(Thread)) {
            CInfoList[Thread] = Info;
        }
        ListMutex->unlock();
    }
}

void Widget::LoadProductDB(CommuniCation* Thread)
{
    SendData((DMan->getProductData()).toJson(),Thread,PD_ALL);
}

void Widget::ProductAdd(CommuniCation *Thread, const QBuffer &MessageData)
{
    DMan->AddProductData(MessageData.data());
    SendData((DMan->getProductData()).toJson(),Thread,PD_ADD);
}

void Widget::ProductDel(CommuniCation *Thread, const QBuffer &MessageData)
{
    DMan->DelProductData(MessageData.data());
    SendData((DMan->getProductData()).toJson(),Thread,PD_DEL);
}

void Widget::ConfirmLogin(CommuniCation *Thread, const QBuffer &MessageData)
{
    QJsonObject   ClientLoginInfo = DMan->IsClient(MessageData.data());
    QJsonDocument doc(ClientLoginInfo);
    QByteArray    LoginInfo = doc.toJson();
    CInfoList[Thread]->setClientData(LoginInfo);
    CInfoList[Thread]->ChangeJsonData();
    SendData(LoginInfo,Thread,CONFIRM);
}

void Widget::Join(const QBuffer &MessageData)
{
    DMan->AddClientData(MessageData.data());
}

void Widget::LoadUserInfo(CommuniCation *Thread)
{
    SendData((DMan->getClientData()).toJson(),Thread,USER_ALL);
}

void Widget::OrderAdd(CommuniCation *Thread, const QBuffer &MessageData)
{
    DMan->AddOrderData(MessageData.data());
}

void Widget::LoadOrderInfo(CommuniCation *Thread)
{
    SendData((DMan->getOrderData()).toJson(),Thread,ORDER_ALL);
}

void Widget::SendData(const QByteArray &Data, CommuniCation *Thread, const qint64 &Comand)
{
    QByteArray Convert   = Data;
    QByteArray Container;

    QDataStream out(&Container,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << qint64(0) << qint64(0) << qint64(0) << "filename";
    Container.append(Convert);

    out.device()->seek(0);
    qint64 dataType = Comand;
    out << dataType << Container.size() << Container.size();
    qDebug() << "send Data";
    qDebug() << Convert;
    QMetaObject::invokeMethod(Thread,"WriteData", Qt::QueuedConnection, Q_ARG(QByteArray, Container));
}

void Widget::DisConnectEvent(QTcpSocket* Socket, CommuniCation* Thread)
{
    if (Socket) {
        ListMutex->lock();
        // QMap에서 ClientInfo*를 빠르게 찾음
        if (CInfoList.contains(Thread)) {
            /*
                Info의 의미
                CInfoList에서 DisConnectSocket를 키로 사용하여
                해당 키에 연결된 값(value), 즉 ClientInfo* 포인터를 가져오는 역할
            */
            ClientInfo* Info = CInfoList.value(Thread);
            /*
             *  romove의 의미
                CInfoList에서 키-값 쌍 자체를 제거. 즉, DisConnectSocket이라는 키에
                연결된 ClientInfo* (값)의 관계를 끊음. 그러나!!
                값으로 저장되어 있던 ClientInfo* 포인터가 가리키는
                실제 ClientInfo 객체의 메모리까지 해제해주지는 않음
            */
            CInfoList.remove(Thread);
            // 가져온 ClientInfo* 포인터를 사용하여 실제 ClientInfo 객체의 메모리를 해제
            delete Info;
            //유효하지 않은 메모리 접근 방지. delete를 썼기때문에 nullptr해준것
            Info = nullptr;
            qDebug() << "QMap에서 클라이언트 제거 완료.";
        }
        ListMutex->unlock();
        // 소켓 삭제 예약
        Socket->deleteLater();
    }
    // UI 업데이트: CInfoList.size() 사용
    InfoLabel->setText(tr("%1 connection is established...").arg(CInfoList.size()));
}

void Widget::ProductModi(CommuniCation* Thread, const QByteArray& MessageData)
{
    DMan->ModiProductData(MessageData.data());
    SendData((DMan->getProductData()).toJson(),Thread,PD_MODI);
}

Widget::~Widget()
{
    ListMutex->lock();
    // 모든 ClientInfo 객체 삭제
    if(CInfoList.size()>0){
        for(QMap<CommuniCation*, ClientInfo*>::iterator it = CInfoList.begin();
             it != CInfoList.end(); ++it) {
            ClientInfo* info = it.value();
            delete info;
        }
        if (NewFile) { // NewFile이 nullptr이 아닌 경우에만 delete
            if (NewFile->isOpen()) {
                NewFile->close(); // 열려있으면 닫기
            }
            delete NewFile;
            NewFile = nullptr; // dangling pointer 방지
        }
        CInfoList.clear();
        delete CInfo;
    }
    ListMutex->unlock();
    delete ListMutex;
}
