#ifndef PRODUCTDB_H
#define PRODUCTDB_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include <database.h>

class ProductDB : public DataBase
{
public:
    ProductDB();
    QByteArray LoadData   ()                           override;
    QByteArray AddData    (const QByteArray &NewData)  override;
    QByteArray ModifyData (const QByteArray &ModiData) override;
    QByteArray DeleteData (const QByteArray &DelData)  override;
    QString    FindLastNum(const QString    &Trace)    override;

private:
    QByteArray ProductData;

};

#endif // PRODUCTDB_H
