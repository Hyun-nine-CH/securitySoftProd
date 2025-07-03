#include "mainwindow_admin.h"
#include "ui_mainwindow_admin.h"

#include "admininfoform_prod.h"

MainWindow_Admin::MainWindow_Admin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow_Admin)
{
    ui->setupUi(this);

    AdminInfoForm_Prod* prodTab = new AdminInfoForm_Prod(this);
    ui->tabWidget->addTab(prodTab, tr("제품 관리"));
}

MainWindow_Admin::~MainWindow_Admin()
{
    delete ui;
}
