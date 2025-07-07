#include <QJsonDocument>
#include <QJsonObject>
#include "clientinfo.h"

ClientInfo::ClientInfo()
{

}

void ClientInfo::ChangeJsonData()
{
    //qDebug() << "Client Info Change " << InfoData;
    if(!InfoData.isEmpty()){
        QByteArray Modify = getClientData();
        qDebug() << "WHat is Modify : " << Modify;
        QJsonDocument doc = QJsonDocument::fromJson(InfoData);
        QJsonParseError parseError;
        // 파싱이 성공했고, 파싱된 데이터가 JSON이 맞는지 확인
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();

            // QJsonObject에서 값을 추출하여 할당
            setClientID(obj.value("ClientId").toString());
            setClientRoomId(obj.value("company").toString());
            qDebug() << "JSON Parsing Succsess";
        } else {
            // 파싱 실패 또는 예상치 못한 JSON 형태 (객체가 아님)일 경우 처리
            // 예: 기본값 설정 또는 오류 로깅
            CId = "UnknownUser";
            RId = "default";
            qDebug() << "Failed to parse ClientInfo InfoData or it's not a JSON object.";
            qDebug() << "JSON 파싱 실패:" << parseError.errorString();
            qDebug() << "받은 데이터:" << InfoData;
        }
        setClientSocket(socket);
    }
}

QString ClientInfo::getClientID(){
    return CId;
}

void ClientInfo::setClientID(const QString &Id)
{
    CId = Id;
}

QString ClientInfo::getClientRoomId(){
    return RId;
}

void ClientInfo::setClientRoomId(const QString &RoomId)
{
    RId = RoomId;
}

QTcpSocket* ClientInfo::getClientSocket(){
    return socket;
}

void ClientInfo::setClientSocket(QTcpSocket *soc)
{
    this->socket = soc;
}

QByteArray ClientInfo::getClientData(){
    return InfoData;
}

void ClientInfo::setClientData(const QByteArray &Data)
{
    InfoData = Data;
}
