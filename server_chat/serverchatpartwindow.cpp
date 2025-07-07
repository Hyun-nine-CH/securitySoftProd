/*
#include "serverchatpartwindow.h"
#include "ui_serverchatpartwindow.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ServerChatPartWindow::ServerChatPartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerChatPartWindow)
    , tcpServer(new QTcpServer(this))
{
    ui->setupUi(this);
    loadClientInfo();
    loadAdminInfo();

    connect(tcpServer, &QTcpServer::newConnection, this, &ServerChatPartWindow::onNewConnection);

    if (!tcpServer->listen(QHostAddress::Any, 50000)) {
        qDebug() << "Server start failed:" << tcpServer->errorString();
    } else {
        qDebug() << "Server started on port 50000";
    }
}

ServerChatPartWindow::~ServerChatPartWindow()
{
    delete ui;
}

void ServerChatPartWindow::loadClientInfo()
{
    QFile file("clients_sample.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open clients_sample.json";
        return;
    }
    clientData = QJsonDocument::fromJson(file.readAll()).array();
    file.close();
}

void ServerChatPartWindow::loadAdminInfo()
{
    QFile file("admin_sample.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open admin_sample.json";
        return;
    }
    adminData = QJsonDocument::fromJson(file.readAll()).array();
    file.close();
}

QString ServerChatPartWindow::getCompanyByCredentials(const QString& id, const QString& pw)
{
    for (const QJsonValue& value : clientData) {
        QJsonObject obj = value.toObject();
        if (obj["id"].toString() == id && obj["pw"].toString() == pw) {
            return obj["company"].toString();
        }
    }
    return "";
}

bool ServerChatPartWindow::isAdmin(const QString& id, const QString& pw)
{
    for (const QJsonValue& value : adminData) {
        QJsonObject obj = value.toObject();
        if (obj["id"].toString() == id && obj["pw"].toString() == pw) {
            return true;
        }
    }
    return false;
}

void ServerChatPartWindow::onNewConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    clientList.append(clientSocket);

    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerChatPartWindow::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServerChatPartWindow::onClientDisconnected);

    qDebug() << "New client connected:" << clientSocket->peerAddress().toString();
}

void ServerChatPartWindow::onReadyRead()
{
    QTcpSocket *senderSocket = qobject_cast<QTcpSocket*>(sender());
    if (!senderSocket) return;

    // Process line by line
    while (senderSocket->canReadLine()) {
        QByteArray data = senderSocket->readLine().trimmed();
        if (data.isEmpty()) continue;

        qDebug() << "[Server RECV]:" << data;

        QJsonDocument doc = QJsonDocument::fromJson(data);

        // Case 1: JSON format (chat message)
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            QString type = obj.value("type").toString();

            if (!socketToName.contains(senderSocket)) {
                qWarning() << "[Server] Ignored JSON message from unauthenticated socket.";
                continue;
            }

            if (type == "chat") {
                QString message = obj.value("message").toString();
                if (message.isEmpty()) continue;

                QString senderName = socketToName.value(senderSocket);

                // Message from an admin
                if (adminSockets.contains(senderSocket)) {
                    QString targetCompany = obj.value("target_company").toString();
                    if (targetCompany.isEmpty()) {
                        qDebug() << "Admin message missing 'target_company'. Ignored.";
                        continue;
                    }

                    QJsonObject newMsg;
                    newMsg["type"] = "chat";
                    newMsg["from"] = senderName + " (Admin)";
                    newMsg["company"] = targetCompany;
                    newMsg["message"] = message;

                    QByteArray newMsgData = QJsonDocument(newMsg).toJson(QJsonDocument::Compact) + "\n";

                    broadcastMessage(targetCompany, newMsgData);
                    broadcastToAllAdmins(newMsgData);
                }
                // Message from a client
                else {
                    QString company = socketToCompany.value(senderSocket);

                    QJsonObject newMsg;
                    newMsg["type"] = "chat";
                    newMsg["from"] = senderName;
                    newMsg["company"] = company;
                    newMsg["message"] = message;

                    QByteArray newMsgData = QJsonDocument(newMsg).toJson(QJsonDocument::Compact) + "\n";

                    broadcastMessage(company, newMsgData);
                    broadcastToAllAdmins(newMsgData);
                }
            }
        }
        // Case 2: Legacy format (login message)
        else if (QString::fromUtf8(data).startsWith("/login ")) {
            QString text = QString::fromUtf8(data);
            QStringList tokens = text.mid(7).split(" ");
            if (tokens.size() < 2) {
                senderSocket->write("LOGIN_FAIL\n");
                continue;
            }

            QString id = tokens[0].trimmed();
            QString pw = tokens[1].trimmed();

            if (isAdmin(id, pw)) {
                adminSockets.append(senderSocket);
                socketToName[senderSocket] = id;
                senderSocket->write("LOGIN_SUCCESS:ADMIN\n");
                qDebug() << "[Login Success] Admin:" << id;
            } else {
                QString company = getCompanyByCredentials(id, pw);
                if (!company.isEmpty()) {
                    socketToCompany[senderSocket] = company;
                    companyToClients[company].append(senderSocket);
                    socketToName[senderSocket] = id;
                    senderSocket->write(QString("LOGIN_SUCCESS:CLIENT:%1\n").arg(company).toUtf8());
                    qDebug() << "[Login Success] Client:" << id << "Company:" << company;
                } else {
                    senderSocket->write("LOGIN_FAIL\n");
                    qDebug() << "[Login Fail] ID/PW mismatch for" << id;
                }
            }
        }
    }
}

void ServerChatPartWindow::broadcastMessage(const QString& company, const QByteArray& data)
{
    if (data.isEmpty()) return;

    const QList<QTcpSocket*>& sockets = companyToClients.value(company);
    for (QTcpSocket* socket : sockets) {
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(data);
        }
    }
}

void ServerChatPartWindow::broadcastToAllAdmins(const QByteArray& data)
{
    if (data.isEmpty()) return;

    for (QTcpSocket* socket : adminSockets) {
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(data);
        }
    }
}

void ServerChatPartWindow::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QString name = socketToName.value(clientSocket);
    QString company = socketToCompany.value(clientSocket);

    qDebug() << "Client disconnected:" << name << "from" << company;

    clientList.removeAll(clientSocket);
    adminSockets.removeAll(clientSocket);
    if (!company.isEmpty()) {
        companyToClients[company].removeAll(clientSocket);
    }
    socketToCompany.remove(clientSocket);
    socketToName.remove(clientSocket);

    clientSocket->deleteLater();
}
*/
#include "serverchatpartwindow.h"
#include "ui_serverchatpartwindow.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// ⭐️ QtCharts 관련 헤더는 여기에만 포함해도 충분합니다.
#include <QtCharts/QValueAxis>

ServerChatPartWindow::ServerChatPartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerChatPartWindow)
    , tcpServer(new QTcpServer(this))
    , m_series(nullptr)
    , m_chart(nullptr)
    , m_chartView(nullptr)
    , m_timer(new QTimer(this))
    , m_messageCount(0)
    , m_timeCounter(0)
{
    ui->setupUi(this);
    loadClientInfo();
    loadAdminInfo();

    setupChart();

    connect(tcpServer, &QTcpServer::newConnection, this, &ServerChatPartWindow::onNewConnection);

    connect(m_timer, &QTimer::timeout, this, &ServerChatPartWindow::updateChart);
    m_timer->start(1000);

    if (!tcpServer->listen(QHostAddress::Any, 50000)) {
        qDebug() << "Server start failed:" << tcpServer->errorString();
    } else {
        qDebug() << "Server started on port 50000";
    }
}

ServerChatPartWindow::~ServerChatPartWindow()
{
    delete ui;
}

void ServerChatPartWindow::setupChart()
{
    // ⭐️ 헤더 파일 수정에 맞춰 'QtCharts::' 네임스페이스를 사용하지 않도록 복원
    m_series = new QLineSeries();
    m_chart = new QChart();

    m_chart->addSeries(m_series);
    m_chart->setTitle("실시간 서버 트래픽 (초당 메시지 수)");
    m_chart->legend()->hide();

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 60);
    // ⭐️ 여기가 문제의 부분이었습니다. "초"를 "sec"으로 수정
    axisX->setLabelFormat("%d sec");
    axisX->setTitleText("시간");
    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 50);
    axisY->setTitleText("메시지 수");
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisY);

    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    this->setCentralWidget(m_chartView);
}

void ServerChatPartWindow::updateChart()
{
    if (m_timeCounter > 60) {
        m_chart->axisX()->setRange(m_timeCounter - 60, m_timeCounter);
    }

    m_series->append(m_timeCounter, m_messageCount);

    m_messageCount = 0;
    m_timeCounter++;
}

void ServerChatPartWindow::onReadyRead()
{
    QTcpSocket *senderSocket = qobject_cast<QTcpSocket*>(sender());
    if (!senderSocket) return;

    while (senderSocket->canReadLine()) {
        QByteArray data = senderSocket->readLine().trimmed();
        if (data.isEmpty()) continue;

        m_messageCount++;

        qDebug() << "[Server RECV]:" << data;

        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            QString type = obj.value("type").toString();

            if (!socketToName.contains(senderSocket)) {
                qWarning() << "[Server] Ignored JSON message from unauthenticated socket.";
                continue;
            }

            if (type == "chat") {
                QString message = obj.value("message").toString();
                if (message.isEmpty()) continue;

                QString senderName = socketToName.value(senderSocket);

                if (adminSockets.contains(senderSocket)) {
                    QString targetCompany = obj.value("target_company").toString();
                    if (targetCompany.isEmpty()) {
                        qDebug() << "Admin message missing 'target_company'. Ignored.";
                        continue;
                    }

                    QJsonObject newMsg;
                    newMsg["type"] = "chat";
                    newMsg["from"] = senderName + " (Admin)";
                    newMsg["company"] = targetCompany;
                    newMsg["message"] = message;

                    QByteArray newMsgData = QJsonDocument(newMsg).toJson(QJsonDocument::Compact) + "\n";

                    broadcastMessage(targetCompany, newMsgData);
                    broadcastToAllAdmins(newMsgData);
                }
                else {
                    QString company = socketToCompany.value(senderSocket);

                    QJsonObject newMsg;
                    newMsg["type"] = "chat";
                    newMsg["from"] = senderName;
                    newMsg["company"] = company;
                    newMsg["message"] = message;

                    QByteArray newMsgData = QJsonDocument(newMsg).toJson(QJsonDocument::Compact) + "\n";

                    broadcastMessage(company, newMsgData);
                    broadcastToAllAdmins(newMsgData);
                }
            }
        }
        else if (QString::fromUtf8(data).startsWith("/login ")) {
            QString text = QString::fromUtf8(data);
            QStringList tokens = text.mid(7).split(" ");
            if (tokens.size() < 2) {
                senderSocket->write("LOGIN_FAIL\n");
                continue;
            }

            QString id = tokens[0].trimmed();
            QString pw = tokens[1].trimmed();

            if (isAdmin(id, pw)) {
                adminSockets.append(senderSocket);
                socketToName[senderSocket] = id;
                senderSocket->write("LOGIN_SUCCESS:ADMIN\n");
                qDebug() << "[Login Success] Admin:" << id;
            } else {
                QString company = getCompanyByCredentials(id, pw);
                if (!company.isEmpty()) {
                    socketToCompany[senderSocket] = company;
                    companyToClients[company].append(senderSocket);
                    socketToName[senderSocket] = id;
                    senderSocket->write(QString("LOGIN_SUCCESS:CLIENT:%1\n").arg(company).toUtf8());
                    qDebug() << "[Login Success] Client:" << id << "Company:" << company;
                } else {
                    senderSocket->write("LOGIN_FAIL\n");
                    qDebug() << "[Login Fail] ID/PW mismatch for" << id;
                }
            }
        }
    }
}

// --- 아래 함수들은 이전 버전과 동일 ---

void ServerChatPartWindow::loadClientInfo()
{
    QFile file("clients_sample.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open clients_sample.json";
        return;
    }
    clientData = QJsonDocument::fromJson(file.readAll()).array();
    file.close();
}

void ServerChatPartWindow::loadAdminInfo()
{
    QFile file("admin_sample.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open admin_sample.json";
        return;
    }
    adminData = QJsonDocument::fromJson(file.readAll()).array();
    file.close();
}

QString ServerChatPartWindow::getCompanyByCredentials(const QString& id, const QString& pw)
{
    for (const QJsonValue& value : clientData) {
        QJsonObject obj = value.toObject();
        if (obj["id"].toString() == id && obj["pw"].toString() == pw) {
            return obj["company"].toString();
        }
    }
    return "";
}

bool ServerChatPartWindow::isAdmin(const QString& id, const QString& pw)
{
    for (const QJsonValue& value : adminData) {
        QJsonObject obj = value.toObject();
        if (obj["id"].toString() == id && obj["pw"].toString() == pw) {
            return true;
        }
    }
    return false;
}

void ServerChatPartWindow::onNewConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    clientList.append(clientSocket);
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerChatPartWindow::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServerChatPartWindow::onClientDisconnected);
    qDebug() << "New client connected:" << clientSocket->peerAddress().toString();
}

void ServerChatPartWindow::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QString name = socketToName.value(clientSocket);
    QString company = socketToCompany.value(clientSocket);

    qDebug() << "Client disconnected:" << name << "from" << company;

    clientList.removeAll(clientSocket);
    adminSockets.removeAll(clientSocket);
    if (!company.isEmpty()) {
        companyToClients[company].removeAll(clientSocket);
    }
    socketToCompany.remove(clientSocket);
    socketToName.remove(clientSocket);

    clientSocket->deleteLater();
}

void ServerChatPartWindow::broadcastMessage(const QString& company, const QByteArray& data)
{
    if (data.isEmpty()) return;

    const QList<QTcpSocket*>& sockets = companyToClients.value(company);
    for (QTcpSocket* socket : sockets) {
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(data);
        }
    }
}

void ServerChatPartWindow::broadcastToAllAdmins(const QByteArray& data)
{
    if (data.isEmpty()) return;

    for (QTcpSocket* socket : adminSockets) {
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(data);
        }
    }
}
