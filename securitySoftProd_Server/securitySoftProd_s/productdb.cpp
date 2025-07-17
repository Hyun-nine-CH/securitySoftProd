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
    NewObj.insert(FileId,QString::number(LastNum));
    New.setObject(NewObj);

    QJsonArray AllArr;
    AllArr = AllDoc.array();
    AllArr.append(NewObj);

    AllDoc.setArray(AllArr);
    //qDebug() << "추가 : "<< AllDoc;
    DbManager->SaveData(FilePath);
}

void ProductDB::ModifyData(const QByteArray &ModiData)
{
    //qDebug() << "원본 NewData:" << NewData;
    QJsonDocument &AllDoc = DbManager->getProductData();

    QJsonDocument New       = QJsonDocument::fromJson(ModiData);
    QJsonObject   modifyObj = New.object();

    int  targetId = modifyObj.value(FileId).toString().toInt();
    bool found    = false;

    QJsonArray AllArray = AllDoc.array();
    for (int i = 0; i < AllArray.size(); i++) {
        QJsonObject item = AllArray[i].toObject();

        // productId가 일치하는 항목 찾기
        if (item.contains(FileId) && item.value(FileId).toString().toInt() == targetId) {
            qDebug() << "일치 인덱스:" << i;
            found = true;

            // 수정할 객체의 모든 키를 순회하며 값 업데이트
            QStringList keys = modifyObj.keys();
            for (const QString &key : keys) {
                // productId는 수정하지 않음 (식별자이므로)
                if (key != FileId) {
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
    DbManager->SaveData(FilePath);
}

void ProductDB::DeleteData(const QByteArray &DelData)
{
    QJsonDocument &AllDoc  = DbManager->getProductData();
    QJsonDocument New      = QJsonDocument::fromJson(DelData);
    QJsonArray    AllArray = AllDoc.array();
    QJsonArray    NewArray;

    if(New.isObject()){ // 한개 삭제할때
        QJsonObject   DelObj = New.object();
        int           TargetId  = DelObj.value(FileId).toString().toInt();

        for (int i = 0; i < AllArray.size(); i++) {
            QJsonObject item = AllArray[i].toObject();

            if (item.contains(FileId) && item.value(FileId).toString().toInt() != TargetId)
                NewArray.append(item);
            else
                qDebug() << "삭제 : " << item.value(FileId).toString().toInt();
        }
    } else if(New.isArray()){ //여러개 삭제할때
        QJsonArray   Delarr = New.array();
        bool         found  = false;

        for (int i = 0; i < AllArray.size(); i++) {
            QJsonObject item = AllArray[i].toObject();
            for (int j = 0; j < Delarr.size(); j++) {
                QJsonObject Del = Delarr[j].toObject();
                if(item.contains(FileId) && \
                   item.value(FileId).toString().toInt() == Del.value(FileId).toString().toInt()){
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
    DbManager->SaveData(FilePath);
}

int ProductDB::FindLastNum(const QJsonDocument &Trace)
{
    int BigNum = 0;

    if (!Trace.isNull() && Trace.isArray()) {
        QJsonArray arr = Trace.array();
        if (arr.isEmpty()) {
            qDebug() << "JSON 배열이 비어있습니다.";
        } else {
            BigNum = arr[0].toObject().value(FileId).toString().toInt();

            for (int i = 1; i < arr.size(); ++i) {
                QJsonObject currentObject = arr[i].toObject();
                int currentFileId = currentObject.value(FileId).toString().toInt();
                if (currentFileId > BigNum)
                    BigNum = currentFileId;
            }
            qDebug() << "JSON Parsing Success. Max FileId found:" << BigNum;
        }
    } else {
        qDebug() << "FindLastNum JSON 파싱 실패: Trace가 유효한 배열이 아니거나 null입니다.";
    }
    return BigNum + 1;
}

