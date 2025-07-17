#include "chatlogdb.h"
#include "datamanager.h"

#include <QDir>
#include <QJsonParseError>

ChatLogDB::ChatLogDB(DataManager *Dm, QObject *parent)
:DataBase(Dm,parent)
{
    Clinfo   = new ClientInfo();
    FilePath = "MesgLogDB.json";
    FileName = "MesgLog DB";
}

QJsonDocument ChatLogDB::LoadData()
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

void ChatLogDB::AddData(const QByteArray &NewData)
{
    //qDebug() << "원본 NewData:" << NewData;
    QJsonDocument &AllDoc = DbManager->getChatLogData();
    int LastNum = FindLastNum(AllDoc);

    QJsonDocument New;
    QJsonObject NewObj = New.object();
    //qDebug() << "Convert objec NewData : " << NewObj.keys();
    NewObj.insert("Id",LastNum);
    NewObj.insert("UserId",getClientInfo()->getClientID());
    NewObj.insert("Chat",QString::fromUtf8(NewData));
    NewObj.insert("Timestamp", \
                  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    New.setObject(NewObj);

    QJsonArray AllArr;
    AllArr = AllDoc.array();
    AllArr.append(NewObj);

    AllDoc.setArray(AllArr);
    //qDebug() << "추가 : "<< AllDoc;
    DbManager->SaveData(FilePath);
}

int ChatLogDB::FindLastNum(const QJsonDocument &Trace)
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

void ChatLogDB::setClientInfo(ClientInfo* UserInfo)
{
    Clinfo = UserInfo;
}

const ClientInfo* ChatLogDB::getClientInfo()
{
    return Clinfo;
}
