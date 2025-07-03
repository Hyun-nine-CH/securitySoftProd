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
    if(infoData.size() > BUFSIZ/2){
        return true;
    }
    return false;

}

// QJsonArray ClientInfoParse(QByteArray *InfoData)
// {

// }
