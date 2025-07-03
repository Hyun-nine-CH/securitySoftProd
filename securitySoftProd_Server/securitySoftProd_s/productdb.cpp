#include "productdb.h"
#include <QDir>
ProductDB::ProductDB()
{
    FilePath = "ProductDB.json";
    FileName = "Product DB";
}

QByteArray ProductDB::LoadProductData()
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

    QDataStream out(&ProductData,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << qint64(0) << qint64(0) << qint64(0) << FileName;

    ProductData.append(data);
    TotalSize += ProductData.size();

    out.device()->seek(0);
    qint64 dataType = 0x02;
    out << dataType << TotalSize << TotalSize;

    return ProductData;

    // // 파싱 오류 확인
    // if (parseError.error != QJsonParseError::NoError) {
    //     qWarning() << "Failed to parse JSON document:" << parseError.errorString();
    //     return;
    // }

    // JSON 문서가 배열인지 확인 (ProductDB.json은 최상위가 배열입니다.)
    //if (jsonDoc.isArray()) {
        //QJsonArray productArray = jsonDoc.array();

        //qDebug() << "--- 제품 데이터 로드 완료 (" << productArray.size() << "개) ---";
        // 배열의 각 요소를 순회하며 제품 정보 출력
        // for (const QJsonValue &value : productArray) {
        //     // 각 요소가 JSON 객체인지 확인
        //     if (value.isObject()) {
        //         QJsonObject productObject = value.toObject();

        //         // 각 필드 조회
        //         QString productId    = productObject["productId"].toString();
        //         QString productName  = productObject["productName"].toString();
        //         double price         = productObject["price"].toDouble(); // 가격은 실수형
        //         int stock            = productObject["stock"].toInt();       // 재고는 정수형
        //         QString description  = productObject["description"].toString();
        //         QString category     = productObject["category"].toString();
        //         QString manufacturer = productObject["manufacturer"].toString();
        //         QString imageUrl     = productObject["imageUrl"].toString();


        //         qDebug() << "------------------------------------";
        //         qDebug() << "제품 ID:" << productId;
        //         qDebug() << "제품명:" << productName;
        //         qDebug() << "가격:" << price << "원";
        //         qDebug() << "재고:" << stock << "개";
        //         qDebug() << "설명:" << description;
        //         qDebug() << "카테고리:" << category;
        //         qDebug() << "제조사:"  << manufacturer;
        //         qDebug() << "이미지 URL:" << imageUrl;
        //     } else {
        //         qWarning() << "JSON 배열 내부에 객체가 아닌 요소가 있습니다.";
        //     }
        // }
        //qDebug() << "--- 제품 데이터 출력 종료 ---";
    //} else {
    //    qWarning() << "JSON 문서의 최상위 요소가 배열이 아닙니다. 예상 형식과 다릅니다.";
    //}
}

// void Widget::openFile()
// {
//     loadSize = byteToWrite = totalSize = 0;
//     outBlock.clear();

//     filename = QFileDialog::getOpenFileName(this);
//     file = new QFile(filename);
//     file->open(QFile::ReadOnly);

//     infoLabel->setText(tr("file %1 is opened").arg(filename));
//     progressDialog->setValue(0);
// }
