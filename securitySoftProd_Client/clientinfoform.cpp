#include "clientinfoform.h"
#include "ui_clientinfoform.h"

ClientInfoForm::ClientInfoForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientInfoForm)
{
    ui->setupUi(this);

    // 🔽 채팅방 탭을 맨 위로 이동시키는 코드
    int chatIndex = ui->toolBox->indexOf(ui->page_2); // page_3이 채팅방인 경우
    if (chatIndex != -1) {
        QWidget* chatPage = ui->toolBox->widget(chatIndex);
        QString chatTitle = ui->toolBox->itemText(chatIndex);

        ui->toolBox->removeItem(chatIndex);
        ui->toolBox->insertItem(0, chatPage, chatTitle);
    }
}

ClientInfoForm::~ClientInfoForm()
{
    delete ui;
}
