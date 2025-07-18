#ifndef FILDB_H
#define FILDB_H

#include "database.h"
#include <QJsonDocument>

class FilDB : public DataBase
{
public:
    FilDB(DataManager *Dm, QObject *parent = nullptr);
    QJsonDocument LoadData     ()                             override;
    void          AddData      (const QByteArray    &NewData) override;
    int           FindLastNum  (const QJsonDocument &Trace)   override;
    bool          SaveData     (const QJsonObject &files,  QJsonDocument &Data);
};

#endif // FILDB_H
