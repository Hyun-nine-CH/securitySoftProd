#ifndef PARSING_H
#define PARSING_H
#include <QJsonDocument>
#include <QJsonArray>

class Parsing
{
public:
    Parsing();
    QJsonArray ClientInfoParse(QByteArray *InfoData);
    bool IsInfo(const QByteArray &infoData);
    bool IsFile(const QByteArray &infoData);
};

#endif // PARSING_H
