#include "admininfoform_ol.h"
#include "ui_admininfoform_ol.h"

AdminInfoForm_OL::AdminInfoForm_OL(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminInfoForm_OL)
{
    ui->setupUi(this);
}

AdminInfoForm_OL::~AdminInfoForm_OL()
{
    delete ui;
}
