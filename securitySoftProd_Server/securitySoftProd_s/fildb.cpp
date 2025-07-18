#include "fildb.h"
#include "datamanager.h"

#include <QDir>
#include <QJsonParseError>
FilDB::FilDB(DataManager *Dm, QObject *parent)
:DataBase(Dm,parent)
{
    FilePath = "file.json";
    FileName = "File DB";
}

QJsonDocument FilDB::LoadData()
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

void FilDB::AddData(const QByteArray &NewData)
{
    QJsonDocument &AllDoc = DbManager->getChatLogData();
    QJsonDocument New;
    QJsonObject NewObj = New.object();
    QJsonDocument files_ = QJsonDocument::fromJson(NewData);
    // type filename filetype data
    qDebug() << "Convert objec NewData : " << NewData;
    if (!files_.isNull() && files_.isObject()) {
        QJsonObject FileObj = files_.object();

        NewObj.insert("id",FileObj["id"].toString());
        NewObj.insert("data",FileObj["data"].toString());
        NewObj.insert("Timestamp", \
                                   QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    } else {
        qDebug() << "유효한 JSON 객체가 아닙니다.";
    }

    New.setObject(NewObj);

    QJsonArray AllArr;
    AllArr = AllDoc.array();
    AllArr.append(NewObj);

    AllDoc.setArray(AllArr);
    qDebug() << "채팅로그 최종 내용:\n" << AllDoc.toJson(QJsonDocument::Indented);
    //qDebug() << "추가 : "<< AllDoc;
    DbManager->SaveData(FilePath, DBType::FILE);
}

int FilDB::FindLastNum(const QJsonDocument &Trace)
{
    return 0;
}

bool FilDB::SaveData(const QJsonObject &files, QJsonDocument &Data)
{
    QJsonDocument &AllDoc = DbManager->getFileData();
    QJsonDocument New;
    QJsonObject NewObj = New.object();

    NewObj.insert("id",files["id"].toString());
    NewObj.insert("data",files["data"].toString());
    NewObj.insert("Timestamp", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    New.setObject(NewObj);

    QJsonArray AllArr;
    AllArr = AllDoc.array();
    AllArr.append(NewObj);

    AllDoc.setArray(AllArr);
    qDebug() << "파일 최종 내용:\n" << AllDoc.toJson(QJsonDocument::Indented);
    //qDebug() << "추가 : "<< ;
    DbManager->SaveData(FilePath, DBType::FILE);

    return true;
}
