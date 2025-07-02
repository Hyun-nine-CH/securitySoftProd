#include "clientinfoform.h"
#include "ui_clientinfoform.h"

ClientInfoForm::ClientInfoForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientInfoForm)
{
    ui->setupUi(this);
}

ClientInfoForm::~ClientInfoForm()
{
    delete ui;
}
