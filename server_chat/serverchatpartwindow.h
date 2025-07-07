/*
#ifndef SERVERCHATPARTWINDOW_H
#define SERVERCHATPARTWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerChatPartWindow; }
QT_END_NAMESPACE

class ServerChatPartWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerChatPartWindow(QWidget *parent = nullptr);
    ~ServerChatPartWindow();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    // ⭐️ UserInfo 구조체를 사용하기 전 원래 함수들로 복원
    void loadClientInfo();
    void loadAdminInfo();
    QString getCompanyByCredentials(const QString& id, const QString& pw);
    bool isAdmin(const QString& id, const QString& pw);

    void broadcastMessage(const QString& company, const QByteArray& data);
    void broadcastToAllAdmins(const QByteArray& data);

    Ui::ServerChatPartWindow *ui;
    QTcpServer *tcpServer;

    // 클라이언트 소켓들을 관리하기 위한 자료구조
    QList<QTcpSocket*> clientList;
    QList<QTcpSocket*> adminSockets;
    QMap<QString, QList<QTcpSocket*>> companyToClients;
    QMap<QTcpSocket*, QString> socketToCompany; // ⭐️ socketToCompany 맵 복원
    QMap<QTcpSocket*, QString> socketToName;

    // JSON 파일에서 읽어온 원본 데이터
    QJsonArray clientData;
    QJsonArray adminData;
};
#endif // SERVERCHATPARTWINDOW_H
*/
#ifndef SERVERCHATPARTWINDOW_H
#define SERVERCHATPARTWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include <QJsonArray>
#include <QTimer>

// ⭐️ Qt Charts 헤더 추가
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerChatPartWindow; }
QT_END_NAMESPACE

// ⭐️ 이 위치로 이동

    class ServerChatPartWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerChatPartWindow(QWidget *parent = nullptr);
    ~ServerChatPartWindow();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void updateChart();

private:
    void loadClientInfo();
    void loadAdminInfo();
    QString getCompanyByCredentials(const QString& id, const QString& pw);
    bool isAdmin(const QString& id, const QString& pw);

    void broadcastMessage(const QString& company, const QByteArray& data);
    void broadcastToAllAdmins(const QByteArray& data);

    void setupChart();

    Ui::ServerChatPartWindow *ui;
    QTcpServer *tcpServer;

    // 클라이언트 소켓 관리 자료구조 (이전과 동일)
    QList<QTcpSocket*> clientList;
    QList<QTcpSocket*> adminSockets;
    QMap<QString, QList<QTcpSocket*>> companyToClients;
    QMap<QTcpSocket*, QString> socketToCompany;
    QMap<QTcpSocket*, QString> socketToName;
    QJsonArray clientData;
    QJsonArray adminData;

    // 차트 관련 멤버 변수 (이전과 동일)
    QLineSeries *m_series;
    QChart *m_chart;
    QChartView *m_chartView;
    QTimer *m_timer;
    int m_messageCount;
    int m_timeCounter;
};
#endif // SERVERCHATPARTWINDOW_H
