#include "productdb.h"
#include <QDir>
ProductDB::ProductDB()
{
    FilePath = "ProductDB.json";
    FileName = "Product DB";
}

QByteArray ProductDB::LoadData()
{
    QFile loadFile(FilePath);
    ProductData.clear();
    TotalSize = 0;
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

QByteArray ProductDB::AddData(const QByteArray &NewData)
{
    QString LastNum = FindLastNum(LoadData());

    QJsonDocument New = QJsonDocument::fromJson(NewData);

    return LoadData();
}

QByteArray ProductDB::ModifyData(const QByteArray &ModiData)
{
    return LoadData();
}

QString ProductDB::FindLastNum(const QString &Trace)
{
    QString LastNum;

    return LastNum;
}

