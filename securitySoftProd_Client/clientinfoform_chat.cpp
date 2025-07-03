#include "clientinfoform_chat.h"
#include "ui_clientinfoform_chat.h"

ClientInfoForm_Chat::ClientInfoForm_Chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientInfoForm_Chat)
{
    ui->setupUi(this);
}

ClientInfoForm_Chat::~ClientInfoForm_Chat()
{
    delete ui;
}
