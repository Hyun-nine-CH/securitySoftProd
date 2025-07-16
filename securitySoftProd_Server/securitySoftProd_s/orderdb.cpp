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
    qDebug() << "원본 NewData:" << NewData;
    QJsonDocument &AllDoc = DbManager->getOrderData();
    //int LastNum = FindLastNum(AllDoc);

    QJsonDocument New = QJsonDocument::fromJson(NewData);
    QJsonObject NewObj = New.object();
    //qDebug() << "Convert objec NewData : " << NewObj.keys();
    //NewObj.insert("ClientId",LastNum);
    New.setObject(NewObj);

    QJsonArray AllArr;
    AllArr = AllDoc.array();
    AllArr.append(NewObj);

    AllDoc.setArray(AllArr);
    qDebug() << "추가 : "<< AllDoc;
    DbManager->SaveOrderData(FilePath);
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

QJsonDocument OrderDB::LoadThatData(int ClientId)
{
    QByteArray jsonData = LoadData().toJson();

    // QJsonDocument를 사용하여 JSON 데이터 파싱
    QJsonParseError parseError;
    QJsonDocument   jsonDoc     = QJsonDocument::fromJson(jsonData, &parseError);

    QJsonArray ThatOrder;
    QJsonArray AllArr = jsonDoc.array();
    for(const QJsonValue& value : AllArr){
        if (value.isObject()) {
            QJsonObject Obj = value.toObject(); // QJsonObject로 변환

            // 객체에 "ClientId" 키가 존재하는지 확인합니다.
            if (Obj.contains("ClientId")) {
                bool ok;
                QJsonValue clientIdValue = Obj.value("ClientId"); // "ClientId" 값 추출
                QVariant tempVariant = clientIdValue.toVariant();
                qlonglong tempClientId = tempVariant.toLongLong(&ok); // int로 변환
                qDebug() << "int 로 변환 : " << tempClientId;
                // ClientId 값이 숫자인지 확인하고 int로 변환합니다.
                if (clientIdValue.isDouble() || clientIdValue.isString()) { // 문자열 형태의 숫자도 처리 가능하도록
                    qDebug() << "클래스 내에서 받아온 클넘 : " << ClientId;
                    if (tempClientId == ClientId) {
                        // 추출한 ClientId가 멤버 변수 m_myClientId와 일치하는지 비교합니다.
                        //qDebug() << "ClientId " << ClientId.toInt() << "와 일치하는 객체를 찾았습니다:" << currentClientId;
                        ThatOrder.append(Obj); // 일치하면 결과 배열에 추가
                        qDebug() << value;
                    }
                } else {
                    qDebug() << "경고: 'ClientId' 값이 숫자 또는 문자열이 아닙니다. 값:" << clientIdValue.toVariant();
                }
            } else {
                qDebug() << "경고: 객체에 'ClientId' 키가 없습니다:" << Obj;
            }
        } else {
            qDebug() << "경고: 배열 요소가 JSON 객체가 아닙니다. 스킵합니다:" << value.toVariant();
        }
    }
    QJsonDocument doc(ThatOrder);
    return doc;
}
