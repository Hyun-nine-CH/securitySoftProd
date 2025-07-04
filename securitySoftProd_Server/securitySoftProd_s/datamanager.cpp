#include "datamanager.h"

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    PDb = new ProductDB();

    Db.insert("Product",PDb);
}
