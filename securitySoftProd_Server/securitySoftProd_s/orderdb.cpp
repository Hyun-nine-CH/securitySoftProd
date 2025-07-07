#include "orderdb.h"
#include "datamanager.h"

#include <QDir>
#include <QJsonParseError>

OrderDB::OrderDB(DataManager *Dm, QObject *parent)
    :DataBase(Dm,parent)
{
    FilePath = "OrderInfoDB.json";
    FileName = "Order DB";
}

QJsonDocument OrderDB::LoadData()
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

    return jsonDoc;
}

void OrderDB::AddData(const QByteArray &NewData)
{
    //qDebug() << "원본 NewData:" << NewData;
    QJsonDocument &AllDoc = DbManager->getOrderData();
    int LastNum = FindLastNum(AllDoc);

    QJsonDocument New = QJsonDocument::fromJson(NewData);
    QJsonObject NewObj = New.object();
    //qDebug() << "Convert objec NewData : " << NewObj.keys();
    NewObj.insert("Id",LastNum);
    New.setObject(NewObj);

    QJsonArray AllArr;
    AllArr = AllDoc.array();
    AllArr.append(NewObj);

    AllDoc.setArray(AllArr);
    //qDebug() << "추가 : "<< AllDoc;
    DbManager->SaveProductData(FilePath);
}

int OrderDB::FindLastNum(const QJsonDocument &Trace)
{
    QJsonParseError parseError;
    int BigNum = 0;
    if (!Trace.isNull() && Trace.isArray()) {
        QJsonArray arr = Trace.array();
        for(int i = 0; i < arr.size()-1; i++) {
            QJsonObject first = arr[i].toObject();
            QJsonObject second = arr[i+1].toObject();
            first.value(FileId).toInt() < second.value(FileId).toInt() ? \
                                                                         BigNum = second.value(FileId).toInt() : BigNum = first.value(FileId).toInt();
        }
        qDebug() << "JSON Parsing Succsess";
    } else
        qDebug() << "FindLastNum JSON 파싱 실패:" << parseError.errorString();

    return BigNum+1;
}
