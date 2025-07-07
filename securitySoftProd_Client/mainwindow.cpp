#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientinfoform_prod.h"
#include "clientinfoform.h"
#include "clientinfoform_chat.h"
#include <QJsonDocument>
#include <QJsonObject>

// 생성자 구현 변경
MainWindow::MainWindow(QTcpSocket* socket, const QString& companyName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_socket(socket)
    , m_companyName(companyName) // 전달받은 회사 이름 저장
{
    ui->setupUi(this);

    // 중앙 메시지 처리 슬롯 연결
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::handleServerMessage);

    // 제품 정보, 주문 정보 탭은 그대로 생성
    ClientInfoForm_Prod* clientProd = new ClientInfoForm_Prod(this);
    ui->tabWidget->addTab(clientProd, tr("제품 정보"));

    ClientInfoForm* clientInfo = new ClientInfoForm(this);
    ui->tabWidget->addTab(clientInfo, tr("주문 정보"));

    // 채팅 탭은 여기서 생성하고 포인터를 저장해 둠
    m_chatTab = new ClientInfoForm_Chat(m_socket, this);
    ui->tabWidget->addTab(m_chatTab, tr("채팅방"));

    ui->tabWidget->setCurrentIndex(0);

    // 탭 전환 시 알림 아이콘 제거 기능은 유지
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (auto chatWidget = qobject_cast<ClientInfoForm_Chat*>(ui->tabWidget->widget(index))) {
            chatWidget->onChatTabActivated();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ⭐️ handleServerMessage 함수를 이전 버전으로 복원
void MainWindow::handleServerMessage()
{
    while (m_socket->canReadLine()) {
        QByteArray data = m_socket->readLine().trimmed();
        if (data.isEmpty()) continue;

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) continue;

        QJsonObject obj = doc.object();
        if (obj.value("type").toString() == "chat") {
            if (obj.value("company").toString() != m_companyName) {
                continue;
            }

            QString from = obj.value("from").toString();
            QString message = obj.value("message").toString();

            if (from.isEmpty()) continue;

            // ⭐️ 이전 버전의 메시지 형식으로 복원
            QString formattedMessage = QString("[%1] %2").arg(from, message);
            m_chatTab->appendMessage(formattedMessage);
        }
    }
}
