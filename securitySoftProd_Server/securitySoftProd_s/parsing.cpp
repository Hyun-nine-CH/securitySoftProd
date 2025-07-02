#include "parsing.h"

Parsing::Parsing()
{

}

bool Parsing::IsInfo(const QByteArray &infoData)
{
    if(infoData.contains("SendInfoData")){
        return true;
    }
    return false;
}

bool Parsing::IsFile(const QByteArray &infoData)
{
    // 최소한 파일 헤더의 첫 16바이트(두 개의 qint64)가 있는지 확인
    QDataStream headerCheckStream(infoData);
    headerCheckStream.setVersion(QDataStream::Qt_5_15); // 클라이언트와 동일한 버전 설정

    qint64 potentialTotalSize;
    qint64 potentialFirstBlockSize;

    headerCheckStream >> potentialTotalSize >> potentialFirstBlockSize;

    // **여기서 추론 시작!**
    // potentialTotalSize가 합리적인 파일 크기처럼 보이고,
    // potentialFirstBlockSize도 유효한 값처럼 보인다면 파일 전송일 가능성이 높음
    if (potentialTotalSize > 100 && // 예를 들어 100바이트 이하는 파일이 아닐 확률 높음
        potentialFirstBlockSize > 0 && potentialFirstBlockSize <= potentialTotalSize){
        return true;
    }
    return false;
}

// QJsonArray ClientInfoParse(QByteArray *InfoData)
// {

// }
