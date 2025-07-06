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
    //qDebug() << "원본 NewData:" << NewData;
    QJsonDocument &AllDoc = DbManager->getProductData();
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

void ProductDB::ModifyData(const QByteArray &ModiData)
{
    //qDebug() << "원본 NewData:" << NewData;
    QJsonDocument &AllDoc = DbManager->getProductData();

    QJsonDocument New       = QJsonDocument::fromJson(ModiData);
    QJsonObject   modifyObj = New.object();

    int  targetId = modifyObj["productId"].toInt();
    bool found    = false;

    QJsonArray AllArray = AllDoc.array();
    for (int i = 0; i < AllArray.size(); i++) {
        QJsonObject item = AllArray[i].toObject();

        // productId가 일치하는 항목 찾기
        if (item.contains("productId") && item["productId"].toInt() == targetId) {
            qDebug() << "일치 인덱스:" << i;
            found = true;

            // 수정할 객체의 모든 키를 순회하며 값 업데이트
            QStringList keys = modifyObj.keys();
            for (const QString &key : keys) {
                // productId는 수정하지 않음 (식별자이므로)
                if (key != "productId") {
                    // 중요: 기존 항목에 해당 키가 있는 경우에만 업데이트
                    if (item.contains(key)) {
                        item[key] = modifyObj[key];
                        //qDebug() << "수정:" << key << "=" << modifyObj[key];
                    } else {
                        //qDebug() << "기존에 없음:" << key;
                    }
                }
            }

            //수정된 객체를 배열에 다시 넣기
            AllArray[i] = item;
            break;
        }
    }
    if (!found) {
        qDebug() << "해당 productId를 가진 상품을 찾을 수 없습니다:" << targetId;
        return;
    }

    AllDoc.setArray(AllArray);
    DbManager->SaveProductData(FilePath);
}

void ProductDB::DeleteData(const QByteArray &DelData)
{
    QJsonDocument &AllDoc  = DbManager->getProductData();
    QJsonDocument New      = QJsonDocument::fromJson(DelData);
    QJsonArray    AllArray = AllDoc.array();
    QJsonArray    NewArray;

    if(New.isObject()){ // 한개 삭제할때
        QJsonObject   DelObj = New.object();
        int           TargetId  = DelObj["productId"].toInt();

        for (int i = 0; i < AllArray.size(); i++) {
            QJsonObject item = AllArray[i].toObject();

            if (item.contains("productId") && item["productId"].toInt() == TargetId)
                continue;

            NewArray.append(item);
        }
    } else if(New.isArray()){ //여러개 삭제할때
        QJsonArray   Delarr = New.array();
        bool         found  = false;

        for (int i = 0; i < AllArray.size(); i++) {
            QJsonObject item = AllArray[i].toObject();
            for (int j = 0; j < Delarr.size(); j++) {
                QJsonObject Del = Delarr[j].toObject();
                if(item.contains("productId") && \
                   item["productId"].toInt() == Del["productId"].toInt()){
                    found = true;
                    break;
                }
            }
            if(!found)
                NewArray.append(item);
            found = false;
        }
    }

    AllDoc.setArray(NewArray);
    DbManager->SaveProductData(FilePath);
}

int ProductDB::FindLastNum(const QJsonDocument &Trace)
{
    QJsonParseError parseError;
    int BigNum = 0;
    if (!Trace.isNull() && Trace.isArray()) {
        QJsonArray arr = Trace.array();
        for(int i = 0; i < arr.size()-1; i++) {
            QJsonObject first = arr[i].toObject();
            QJsonObject second = arr[i+1].toObject();
            first.value("productId").toInt() < second.value("productId").toInt() ? \
            BigNum = second.value("productId").toInt() : BigNum = first.value("productId").toInt();
        }
        qDebug() << "JSON Parsing Succsess";
    } else
        qDebug() << "FindLastNum JSON 파싱 실패:" << parseError.errorString();

    return BigNum+1;
}

