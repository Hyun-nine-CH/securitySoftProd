#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "clientinfoform_prod.h"
#include "clientinfoform.h"
#include "clientinfoform_chat.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ClientInfoForm_Prod* clientProd = new ClientInfoForm_Prod(this);
    ui->tabWidget->addTab(clientProd, tr("제품 정보"));

    ClientInfoForm* clientInfo = new ClientInfoForm(this);
    ui->tabWidget->addTab(clientInfo, tr("주문 정보"));

    ClientInfoForm_Chat* clientChat = new ClientInfoForm_Chat(this);
    ui->tabWidget->addTab(clientChat, tr("채팅방"));

    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
