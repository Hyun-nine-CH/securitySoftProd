#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    virtual  QByteArray LoadData   ()                           = 0;
    virtual  QByteArray AddData    (const QByteArray &NewData)  = 0;
    virtual  QByteArray ModifyData (const QByteArray &ModiData){return ModiData;};
    virtual  QByteArray DeleteData (const QByteArray &DelData) {return DelData;};
    virtual  QString    FindLastNum(const QString    &Trace)   {return Trace;};

protected:
    QString    FilePath;
    QString    FileName;
    qint64     TotalSize;

};

#endif // DATABASE_H
