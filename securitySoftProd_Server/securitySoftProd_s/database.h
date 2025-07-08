#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

class DataManager;

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(DataManager *Dm, QObject *parent = nullptr);
    virtual  QJsonDocument LoadData   ()                              = 0;
    virtual  void          AddData    (const QByteArray    &NewData) {};
    virtual  void          ModifyData (const QByteArray    &ModiData){};
    virtual  void          DeleteData (const QByteArray    &DelData) {};
    virtual  int           FindLastNum(const QJsonDocument &Trace)   {return 1;};

protected:
    QString     FilePath;
    QString     FileName;
    qint64      TotalSize;
    DataManager *DbManager;

};

#endif // DATABASE_H
