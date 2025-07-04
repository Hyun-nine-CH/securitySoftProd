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

private:
    QMap<QString,DataBase*> Db;
    ProductDB *PDb;

};

#endif // DATAMANAGER_H
