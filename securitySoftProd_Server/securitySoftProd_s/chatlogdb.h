#ifndef CHATLOGDB_H
#define CHATLOGDB_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "database.h"

class ChatLogDB : public DataBase
{
public:
    ChatLogDB(DataManager *Dm, QObject *parent = nullptr);
    QJsonDocument LoadData   ()                              override;
    void          AddData    (const QByteArray    &NewData)  override;
    int           FindLastNum(const QJsonDocument &Trace)    override;

private:
    QByteArray ChatLogData;
    const QString FileId = "제품 번호";

};

#endif // CHATLOGDB_H
