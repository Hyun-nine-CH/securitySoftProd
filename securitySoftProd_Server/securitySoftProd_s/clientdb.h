#ifndef CLIENTDB_H
#define CLIENTDB_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "database.h"

class ClientDB : public DataBase
{
public:
    ClientDB(DataManager *Dm, QObject *parent = nullptr);
    QJsonDocument LoadData   ()                              override;
    void          AddData    (const QByteArray &NewData)     override;
    QJsonObject Confirm      (const QByteArray &IdPwData);
    QJsonObject IdCheck      (const QByteArray &IdPwData);

private:
    QByteArray ClientData;
};

#endif // CLIENTDB_H
