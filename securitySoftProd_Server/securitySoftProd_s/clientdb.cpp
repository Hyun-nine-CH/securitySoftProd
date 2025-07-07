#include "clientdb.h"
#include "datamanager.h"

#include <QDir>
#include <QJsonParseError>

ClientDB::ClientDB(DataManager *Dm, QObject *parent)
    :DataBase(Dm,parent)
{
    FilePath = "ClientInfoDB.json";
    FileName = "Client DB";
}

QJsonDocument ClientDB::LoadData()
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

void ClientDB::AddData(const QByteArray &NewData)
{
    //qDebug() << "원본 NewData:" << NewData;
    QJsonDocument &AllDoc = DbManager->getClientData();
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
    DbManager->SaveClientData(FilePath);
}

QJsonObject ClientDB::Confirm(const QByteArray &IdPwData)
{
    QJsonDocument &AllDoc  = DbManager->getClientData();
    QJsonDocument New      = QJsonDocument::fromJson(IdPwData);
    QJsonObject   ConFObj  = New.object();
    QString       TargetId = ConFObj["id"].toString();
    QString       TargetPw = ConFObj["pw"].toString();
    QJsonArray    AllArray = AllDoc.array();
    if (New.isNull() || !New.isObject()) { // 입력 데이터가 유효한 JSON 객체인지 확인
        qWarning() << "입력 데이터가 유효한 JSON 객체가 아닙니다.";
    }
    // 키 존재 여부 확인 후 값 추출
    if (!ConFObj.contains("id") || !ConFObj.contains("pw")) {
        qWarning() << "입력 데이터에 'id' 또는 'pw' 키가 없습니다.";
    }

    if (AllDoc.isNull() || !AllDoc.isArray()) { // DB 문서가 유효한 JSON 배열인지 확인
        qWarning() << "클라이언트 데이터가 유효한 JSON 배열이 아닙니다.";
    }

    for (int i = 0; i < AllArray.size(); i++) {
        QJsonValueRef itemRef = AllArray[i]; // QJsonValueRef를 사용하여 불필요한 복사 방지
        if (!itemRef.isObject()) { // 배열의 각 항목이 객체인지 확인
            qWarning() << "배열의 " << i << "번째 항목이 객체가 아닙니다. 스킵합니다.";
            continue; // 객체가 아니면 다음 항목으로 넘어감
        }
        QJsonObject item = itemRef.toObject();

        // "Id" 키 존재 여부 확인 후 비교
        if (item.contains("id") && item["id"].toString() == TargetId) {
            qDebug() << "일치 인덱스:" << i;
            // "pw" 키 존재 여부 확인 후 비교
            if (item.contains("pw") && item["pw"].toString() == TargetPw) {
                // ID와 PW 모두 일치하면 즉시 true 반환
                return item;
            }
        }
        // ID가 일치하지 않거나, ID는 일치해도 PW가 일치하지 않으면 계속 루프를 진행하여 다음 항목을 확인합니다.
    }
    QJsonDocument Err;
    Err.object().insert("False","Not Client");
    // 루프를 모두 돌았는데도 일치하는 ID/PW 조합을 찾지 못하면 false 반환
    return Err.object();
}
