#include "productdb.h"
#include "datamanager.h"

#include <QDir>
#include <QJsonParseError>

ProductDB::ProductDB(DataManager *Dm, QObject *parent)
    :DataBase(Dm,parent)
{ 
    FilePath = "ProductDB.json";
    FileName = "Product DB";
}

QByteArray ProductDB::SendData()
{
    ProductData.clear();
    TotalSize = 0;
    // QJsonDocument를 사용하여 JSON 데이터 파싱
    QByteArray data = (DbManager->getProductData()).toJson();
    QDataStream out(&ProductData,QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_15);
    out << qint64(0) << qint64(0) << qint64(0) << FileName;

    ProductData.append(data);
    TotalSize += ProductData.size();

    out.device()->seek(0);
    qint64 dataType = 0x02;
    out << dataType << TotalSize << TotalSize;

    return ProductData;
}

QJsonDocument ProductDB::LoadData()
{
    QFile loadFile(FilePath);

    // 파일 열기 시도
    if (!loadFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file:" << FilePath;
        qDebug() << "Attempting to open file from relative path (./):" << QDir::currentPath() + QDir::separator() + "ProductDB.json";
    }

    // 파일에서 모든 데이터 읽기
    QByteArray jsonData = loadFile.readAll();
    loadFile.close(); // 파일 사용이 끝났으니 닫습니다.

    // QJsonDocument를 사용하여 JSON 데이터 파싱
    QJsonParseError parseError;
    QJsonDocument   jsonDoc     = QJsonDocument::fromJson(jsonData, &parseError);
    QByteArray      data        = jsonDoc.toJson();

    return jsonDoc;
}

void ProductDB::AddData(const QByteArray &NewData)
{
    qDebug() << "원본 NewData:" << NewData;
    QJsonDocument &AllDoc = DbManager->getProductData();
    int LastNum = FindLastNum(AllDoc);

    QJsonDocument New = QJsonDocument::fromJson(NewData);
    QJsonObject NewObj = New.object();
    qDebug() << "Convert objec NewData : " << NewObj.keys();
    NewObj.insert("Id",LastNum);
    New.setObject(NewObj);

    QJsonArray AllArr;
    AllArr = AllDoc.array();
    AllArr.append(NewObj);

    AllDoc.setArray(AllArr);
    //qDebug() << "추가 : "<< AllDoc;
    DbManager->SaveProductData(FilePath);
    //return LoadData();
}

void ProductDB::ModifyData(const QByteArray &ModiData)
{
    //return LoadData();
}

void ProductDB::DeleteData(const QByteArray &DelData)
{

}

int ProductDB::FindLastNum(const QJsonDocument &Trace)
{
    QJsonParseError parseError;
    int BigNum = 0;
    if (!Trace.isNull() && Trace.isObject()) {
        QJsonArray arr = Trace.array();
        for(int i = 0; i < arr.size()-1; i++) {
            QJsonObject first = arr[i].toObject();
            QJsonObject second = arr[i+1].toObject();
            first.value("id").toInt() < second.value("id").toInt() ? \
            BigNum = second.value("id").toInt() : BigNum = first.value("id").toInt();
        }
        qDebug() << "JSON Parsing Succsess";
    } else
        qDebug() << "JSON 파싱 실패:" << parseError.errorString();

    return BigNum+1;
}

