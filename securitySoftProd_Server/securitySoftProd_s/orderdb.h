#ifndef ORDERDB_H
#define ORDERDB_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "database.h"

class OrderDB : public DataBase
{
public:
    OrderDB(DataManager *Dm, QObject *parent = nullptr);
    QJsonDocument LoadData    ()                              override;
    void          AddData     (const QByteArray    &NewData)  override;
    int           FindLastNum (const QJsonDocument &Trace)    override;
    QJsonDocument LoadThatData(int ClientId);
private:
    QByteArray OrderData;
    const QString FileId = "주문 번호";
};

#endif // ORDERDB_H
