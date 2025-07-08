#ifndef PRODUCTDB_H
#define PRODUCTDB_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "database.h"

class ProductDB : public DataBase
{
public:
    ProductDB(DataManager *Dm, QObject *parent = nullptr);
    QJsonDocument LoadData   ()                              override;
    void          AddData    (const QByteArray    &NewData)  override;
    void          ModifyData (const QByteArray    &ModiData) override;
    void          DeleteData (const QByteArray    &DelData)  override;
    int           FindLastNum(const QJsonDocument &Trace)    override;

private:
    QByteArray ProductData;
    const QString FileId = "제품 번호";
};

#endif // PRODUCTDB_H
