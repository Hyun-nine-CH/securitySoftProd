#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>

#include "database.h"
#include "productdb.h"

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);
    // 각 데이터셋을 파일에 저장하는 메서드
    bool SaveProductData(const QString &filePath);
    // bool SaveClientData(const QString &filePath);
    // bool SaveOrderData(const QString &filePath);
    // bool SaveChatLogData(const QString &filePath);

    // 각 데이터셋에 대한 접근자 (getter)
    QJsonDocument& getProductData();
    // QJsonDocument& getClientData();
    // QJsonDocument& getOrderData();
    // QJsonDocument& getChatLogData();
    void AddProductData(const QByteArray &NewData);

private:
    QMap<QString,DataBase*> Db;
    ProductDB *PDb;

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
