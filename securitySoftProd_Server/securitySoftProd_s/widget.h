#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void clientConnect();
    void echoData();

private:
    QLabel *infoLabel;
    QTcpServer *tcpServer;
};
#endif // WIDGET_H
