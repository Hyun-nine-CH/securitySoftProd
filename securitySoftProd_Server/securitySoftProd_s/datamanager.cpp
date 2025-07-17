#include "datamanager.h"

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    PDb = new ProductDB(this);
    CDb = new ClientDB(this);
    ODb = new OrderDB(this);
    MDb = new ChatLogDB(this);

    Db.insert("Product"   ,PDb);
    Db.insert("Client"    ,CDb);
    Db.insert("Order"     ,ODb);
    Db.insert("MesgLog"   ,MDb);

    if(LoadProductData() && LoadChatLogData() &&
       LoadClientData()  && LoadOrderData() ){
        qDebug() << "Load All DB";
    }else{
        qDebug() << "Fail DB";
    }
    connect(MDb,&ChatLogDB::SendAddData,this,&DataManager::ReceiveAddData);
}

bool DataManager::SaveData(const QString &filePath, DBType Type,QSharedPointer<ClientInfo> userInfo)
{
    bool isChatLog = false;
    if(!userInfo.isNull())
        isChatLog = true;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "파일을 열 수 없습니다: " << filePath;
        return false;
    }
    QJsonDocument Data;
    switch (Type) {
    case DBType::ORDER:
        Data = getOrderData();
        break;
    case DBType::PRODUCT:
        Data = getProductData();
        break;
    case DBType::CLIENT:
        Data = getClientData();
        break;
    case DBType::CHATLOG:
        Data = getChatLogData();
        break;
    default:
        qDebug() << "타입이 잘못 되었습니다";
        break;
    }
    file.write(Data.toJson(QJsonDocument::Indented));
    file.close();
    if(isChatLog)
        emit ChatLogSaveFinished(AddChatData,ChatroomId,userInfo);


    return true;
}

bool DataManager::LoadProductData()
{
    ProductData = PDb->LoadData();
    if(!(ProductData.isEmpty())){
        qDebug() << "Load Product Data";
        return true;
    }
    return false;
}

bool DataManager::LoadClientData()
{
    ClientData = CDb->LoadData();
    if(!(ClientData.isEmpty())){
        qDebug() << "Load Client Data";
        return true;
    }
    return false;
}

bool DataManager::LoadOrderData()
{
    OrderData = ODb->LoadData();
    if(!(OrderData.isEmpty())){
        qDebug() << "Load Order Data";
        return true;
    }
    return false;
}

bool DataManager::LoadChatLogData()
{
    ChatLogData = MDb->LoadData();
    if(!(ChatLogData.isEmpty())){
        qDebug() << "Load MesgLog Data";
        return true;
    }
    return false;
}

void DataManager::ReceiveAddData(QByteArray add, QString rId)
{
    this->AddChatData = add;
    this->ChatroomId  = rId;
}

void DataManager::AddProductData(const QByteArray &NewData)
{
    PDb->AddData(NewData);
}

void DataManager::ModiProductData(const QByteArray &ModiData)
{
    PDb->ModifyData(ModiData);
}

void DataManager::DelProductData(const QByteArray &DelData)
{
    PDb->DeleteData(DelData);
}

void DataManager::AddClientData(const QByteArray &NewData)
{
    CDb->AddData(NewData);
}

QJsonObject DataManager::IsClient(const QByteArray &IdPwData)
{
    return CDb->Confirm(IdPwData);
}

QJsonObject DataManager::IsId(const QByteArray &IdPwData)
{
    return CDb->IdCheck(IdPwData);
}

void DataManager::AddOrderData(const QByteArray &NewData)
{
    ODb->AddData(NewData);
}

QJsonDocument DataManager::LoadThatOrderData(int ClientId)
{
    return ODb->LoadThatData(ClientId);
}

void DataManager::AddChatLogData(const QBuffer &NewData, QSharedPointer<ClientInfo> UserInfo)
{
    QByteArray messageCopy = NewData.data();
    //qDebug() << "서버에서 받은 chat mesg : " << messageCopy;
    QJsonDocument Mesg = QJsonDocument::fromJson(messageCopy);
    QJsonObject MesgObj = Mesg.object();
    QByteArray M = MesgObj.value("message").toString().toUtf8();
    MDb->setClientInfo(UserInfo);
    MDb->AddData(M);
}

void DataManager::AdminAddChatLogData(const QBuffer &NewData, QString RoomId)
{
    QByteArray messageCopy = NewData.data();
    //qDebug() << "서버에서 받은 chat mesg : " << messageCopy;
    QJsonDocument Mesg = QJsonDocument::fromJson(messageCopy);
    QJsonObject MesgObj = Mesg.object();
    QByteArray M = MesgObj.value("message").toString().toUtf8();
    MDb->AdminAddData(M,RoomId);
}

void DataManager::setChatLogUserInfo(QSharedPointer<ClientInfo> UserInfo)
{
    MDb->setClientInfo(UserInfo);
}

QJsonDocument &DataManager::getProductData()
{
    return ProductData;
}

QJsonDocument &DataManager::getClientData()
{
    return ClientData;
}

QJsonDocument &DataManager::getOrderData()
{
    return OrderData;
}

QJsonDocument &DataManager::getChatLogData()
{
    return ChatLogData;
}
