#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "clientinfoform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ClientInfoForm* clientInfo = new ClientInfoForm(this);
    ui->tabWidget->addTab(clientInfo, tr("주문 정보"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
