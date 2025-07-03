#include "clientinfoform_prod.h"
#include "ui_clientinfoform_prod.h"

ClientInfoForm_Prod::ClientInfoForm_Prod(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientInfoForm_Prod)
{
    ui->setupUi(this);
}

ClientInfoForm_Prod::~ClientInfoForm_Prod()
{
    delete ui;
}
