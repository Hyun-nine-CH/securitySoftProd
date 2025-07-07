#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpSocket>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void echoData();
    void sendData();
    void goOnSend(qint64);

private:
    QTextEdit *message;
    QLineEdit *inputLine;
    QTcpSocket *clientSocket;
    QByteArray outBlock;
    QByteArray ContainData;
    QByteArray byteArray;
    QString filename;
    qint64 loadSize;
    qint64 byteToWrite;
    qint64 totalSize;
};
#endif // WIDGET_H
