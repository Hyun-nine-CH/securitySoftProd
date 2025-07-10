#include "clientinfoform.h"
#include "ui_clientinfoform.h"
#include <QMessageBox>

ClientInfoForm::ClientInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientInfoForm)
{
    ui->setupUi(this);
    // '주문하기' 버튼의 objectName이 'pushButton_order'라고 가정
    connect(ui->pushButton_order, &QPushButton::clicked, this, &ClientInfoForm::on_pushButton_order_clicked);
}

ClientInfoForm::~ClientInfoForm()
{
    delete ui;
}

// '주문하기' 버튼 클릭 시
void ClientInfoForm::on_pushButton_order_clicked()
{
    // 스크린샷의 UI 객체 이름(objectName)을 사용
    QString productName = ui->ProdName_2->text().trimmed();
    QString price = ui->Price_2->text().trimmed();
    QString dueDate = ui->Due_2->text().trimmed();

    if (productName.isEmpty() || price.isEmpty() || dueDate.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "모든 주문 정보를 입력해주세요.");
        return;
    }

    // 주문 정보를 JSON 객체로 구성
    QJsonObject orderData;
    orderData["productName"] = productName;
    orderData["price"] = price.toInt(); // 가격은 숫자로 변환
    orderData["dueDate"] = dueDate;

    // MainWindow에 주문 요청 시그널 전송
    emit orderSubmitted(orderData);

    // 입력란 초기화
    ui->ProdName_2->clear();
    ui->Price_2->clear();
    ui->Due_2->clear();
}
