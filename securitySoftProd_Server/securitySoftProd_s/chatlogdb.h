#ifndef CHATLOGDB_H
#define CHATLOGDB_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "database.h"
#include "clientinfo.h"

class ChatLogDB : public DataBase
{
    Q_OBJECT

public:
    ChatLogDB(DataManager *Dm, QObject *parent = nullptr);

    QJsonDocument               LoadData     ()                                             override;
    void                        AddData      (const QByteArray    &NewData)                 override;
    int                         FindLastNum  (const QJsonDocument &Trace)                   override;
    void                        setClientInfo(QSharedPointer<ClientInfo> UserInfo);
    void                        AdminAddData (const QByteArray    &NewData, QString RoomId);
    QSharedPointer<ClientInfo>  getClientInfo();

private:
    QByteArray ChatLogData;
    QSharedPointer<ClientInfo> Clinfo;
    const QString FileId = "LogId";

signals:
    void SendAddData(QByteArray Add, QString RId);
};

#endif // CHATLOGDB_H
