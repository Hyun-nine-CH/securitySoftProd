#include "datamanager.h"

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    PDb = new ProductDB(this);

    Db.insert("Product",PDb);
    if(LoadProductData() && LoadChatLogData() &&
       LoadClientData()  && LoadOrderData()){

    }
}

bool DataManager::LoadProductData()
{
    ProductData = PDb->LoadData();
    if(!(ProductData.isEmpty())){
        return true;
    }
    return false;
}

void DataManager::AddProductData(const QByteArray &NewData)
{
    PDb->AddData(NewData);
}

void DataManager::ModiProductData(const QByteArray &ModiData)
{
    PDb->ModifyData(ModiData);
}

bool DataManager::LoadClientData()
{
    return true;
}

bool DataManager::LoadOrderData()
{
    return true;
}

bool DataManager::LoadChatLogData()
{
    return true;
}

bool DataManager::SaveProductData(const QString &filePath)
{
    // 파일 저장 로직
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "파일을 열 수 없습니다: " << filePath;
        return false;
    }

    // 수정된 데이터 가져오기
    QJsonDocument productData = getProductData();
    //qDebug() << "세이브 해라 : " << productData;
    // JSON 데이터를 파일에 쓰기
    file.write(productData.toJson(QJsonDocument::Indented)); // 들여쓰기 포맷 적용

    file.close();
    return true;
}

// bool DataManager::SaveClientData(const QString &filePath)
// {
//     return true;
// }

// bool DataManager::SaveOrderData(const QString &filePath)
// {
//     return true;
// }

// bool DataManager::SaveChatLogData(const QString &filePath)
// {
//     return true;
// }

QJsonDocument &DataManager::getProductData()
{
    return ProductData;
}

// QJsonDocument &DataManager::getClientData()
// {

// }

// QJsonDocument &DataManager::getOrderData()
// {

// }

// QJsonDocument &DataManager::getChatLogData()
// {

// }
