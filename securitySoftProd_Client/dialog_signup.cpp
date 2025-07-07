#include "dialog_signup.h"
#include "ui_dialog_signup.h"

Dialog_SignUp::Dialog_SignUp(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_SignUp)
{
    ui->setupUi(this);
}

Dialog_SignUp::~Dialog_SignUp()
{
    delete ui;
}
