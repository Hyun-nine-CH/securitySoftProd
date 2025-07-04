#ifndef PRODUCTDB_H
#define PRODUCTDB_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
class ProductDB
{
public:
    ProductDB();
    QByteArray LoadData();
    QByteArray ModifyData();

private:
    QString    FilePath;
    QByteArray ProductData;
    QString    FileName;
    qint64     TotalSize;
};

#endif // PRODUCTDB_H
