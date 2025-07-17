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

    if(LoadProductData() && LoadChatLogData() &&
       LoadClientData()  && LoadOrderData()){
        qDebug() << "Load All DB";
    }else{
        qDebug() << "Fail DB";
    }
}

bool DataManager::SaveData(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "파일을 열 수 없습니다: " << filePath;
        return false;
    }
    QJsonDocument Data = getOrderData();
    //qDebug() << "주문정보 세이브 해라 : " << OrderData;
    file.write(Data.toJson(QJsonDocument::Indented));
    file.close();
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
        qDebug() << "Load ChatLog Data";
        return true;
    }
    return false;
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

void DataManager::AddChatLogData(const QByteArray &NewData, ClientInfo* UserInfo)
{
    MDb->setClientInfo(UserInfo);
    MDb->AddData(NewData);
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
