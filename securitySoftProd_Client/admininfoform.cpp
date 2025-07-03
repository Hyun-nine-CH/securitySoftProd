#include "admininfoform.h"
#include "ui_admininfoform.h"

AdminInfoForm::AdminInfoForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminInfoForm)
{
    ui->setupUi(this);
}

AdminInfoForm::~AdminInfoForm()
{
    delete ui;
}
