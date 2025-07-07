#include "mainwindow_admin.h"
#include "ui_mainwindow_admin.h"
#include "admininfoform_prod.h"
#include "admininfoform.h"
#include "admininfoform_chat.h"
#include <QJsonDocument>
#include <QJsonObject>

    MainWindow_Admin::MainWindow_Admin(QTcpSocket* socket, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow_Admin)
    , m_socket(socket)
{
    ui->setupUi(this);

    // ⭐️ 중앙 메시지 처리 슬롯 연결
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow_Admin::handleServerMessage);

    // 고객 관리, 제품 관리 탭은 그대로 생성
    AdminInfoForm* clientTab = new AdminInfoForm(this);
    ui->tabWidget->addTab(clientTab, tr("고객 관리"));

    AdminInfoForm_Prod* prodTab = new AdminInfoForm_Prod(this);
    ui->tabWidget->addTab(prodTab, tr("제품 관리"));

    // ⭐️ 채팅 탭은 메시지가 왔을 때 동적으로 생성하므로 초기 생성 코드 삭제

    // 탭 전환 시 알림 아이콘 제거 기능은 유지
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (auto chatWidget = qobject_cast<AdminInfoForm_Chat*>(ui->tabWidget->widget(index))) {
            chatWidget->onChatTabActivated();
        }
    });
}

MainWindow_Admin::~MainWindow_Admin()
{
    delete ui;
}

void MainWindow_Admin::handleServerMessage()
{
    while (m_socket->canReadLine()) {
        QByteArray data = m_socket->readLine().trimmed();
        if (data.isEmpty()) continue;

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) continue;

        QJsonObject obj = doc.object();
        QString type = obj.value("type").toString();

        if (type == "chat") {
            QString company = obj.value("company").toString();
            QString from = obj.value("from").toString();
            QString message = obj.value("message").toString();

            if (company.isEmpty() || from.isEmpty()) continue;

            // 1. 해당 회사의 채팅 탭이 없으면 새로 생성 후 전환
            createOrSwitchToChatTab(company);

            // 2. 맵에서 올바른 채팅 탭을 찾아 메시지 전달
            if(m_chatTabs.contains(company)) {
                AdminInfoForm_Chat* chatTab = m_chatTabs.value(company);
                // ⭐️ 이전 버전의 메시지 형식으로 복원
                QString formattedMessage = QString("[%1] %2").arg(from, message);
                chatTab->appendMessage(formattedMessage);
            }
        }
    }
}

void MainWindow_Admin::createOrSwitchToChatTab(const QString& companyName)
{
    // 이미 해당 회사의 탭이 있다면 거기로 이동만 함
    if (m_chatTabs.contains(companyName)) {
        ui->tabWidget->setCurrentWidget(m_chatTabs.value(companyName));
        return;
    }

    // 새 채팅 탭 생성
    AdminInfoForm_Chat* chatTab = new AdminInfoForm_Chat(m_socket, companyName, this);
    m_chatTabs.insert(companyName, chatTab);

    int newIndex = ui->tabWidget->addTab(chatTab, QString("%1 채팅").arg(companyName));
    ui->tabWidget->setCurrentIndex(newIndex);
}
