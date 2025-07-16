#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>

#include "database.h"
#include "productdb.h"
#include "clientdb.h"
#include "orderdb.h"
#include "chatlogdb.h"

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);

    // 각 데이터셋에 대한 접근자 (getter)
    QJsonDocument& getProductData();
    QJsonDocument& getClientData(); //전체 고객 정보
    QJsonDocument& getOrderData();
    QJsonDocument& getChatLogData();
    //ProductDB
    bool SaveProductData(const QString    &filePath);
    void AddProductData (const QByteArray &NewData);
    void ModiProductData(const QByteArray &ModiData);
    void DelProductData (const QByteArray &DelData);

    //Client DB
    bool        SaveClientData(const QString    &filePath);
    void        AddClientData (const QByteArray &NewData);
    QJsonObject IsClient      (const QByteArray &IdPwData);
    QJsonObject IsId          (const QByteArray &IdPwData);

    //Order DB
    bool SaveOrderData(const QString    &filePath);
    void AddOrderData (const QByteArray &NewData);
    QJsonDocument LoadThatOrderData(int ClientId);

    //ChatLog DB
    bool SaveChatLogData(const QString    &filePath);
    void AddChatLogData (const QByteArray &NewData, ClientInfo* UserInfo);

private:
    QMap<QString,DataBase*> Db;
    ProductDB *PDb;
    ClientDB  *CDb;
    OrderDB   *ODb;
    ChatLogDB *MDb;

    QJsonDocument  ProductData;
    QJsonDocument  ClientData;
    QJsonDocument  OrderData;
    QJsonDocument  ChatLogData;

    // 각 데이터셋을 파일에서 로드하는 메서드
    bool LoadProductData();
    bool LoadClientData();
    bool LoadOrderData();
    bool LoadChatLogData();

};

#endif // DATAMANAGER_H
