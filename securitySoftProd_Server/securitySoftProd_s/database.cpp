#include "database.h"

DataBase::DataBase(DataManager *Dm, QObject *parent)
    : QObject{parent}, DbManager(Dm)
{

}
