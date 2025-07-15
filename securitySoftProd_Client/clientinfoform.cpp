#include "clientinfoform.h"
#include "ui_clientinfoform.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include "Protocol.h"
#include "communication.h"
ClientInfoForm::ClientInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientInfoForm)
{
    ui->setupUi(this);

    // // 테이블 위젯 설정
    // ui->tableWidget->setColumnCount(8); // 회사명, 부서명, 담당자명, 폰 번호, 주소, 구매한 제품, 구매한 가격, 만료일
    // QStringList headers;
    // headers << "회사명" << "부서명" << "담당자명" << "폰 번호" << "주소" << "구매한 제품" << "구매한 가격" << "만료일";
    // ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setColumnCount(3); // 제품명, 가격, 납기일
    QStringList headers;
    headers << "제품명" << "가격" << "납기일";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    // 테이블 위젯 열 너비 설정
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 버튼 연결
    //connect(ui->pushButton_order, &QPushButton::clicked, this, &ClientInfoForm::on_pushButton_order_clicked);
    //connect(ui->pushButton_search, &QPushButton::clicked, this, &ClientInfoForm::on_pushButton_search_clicked);
    //connect(ui->pushButton_Reset, &QPushButton::clicked, this, &ClientInfoForm::on_pushButton_Reset_clicked);
    //통신클래스 연결
    connect(this, &ClientInfoForm::productListRequested,Communication::getInstance(),&Communication::RequestProductInfo);
    connect(Communication::getInstance(), &Communication::ReceiveProductInfo,this,&ClientInfoForm::displayProductList);
}

ClientInfoForm::~ClientInfoForm()
{
    delete ui;
}

// void ClientInfoForm::on_pushButton_order_clicked()
// {
//     // // 주문 정보 수집
//     // QString productName = ui->ProdName_2->text().trimmed();
//     // QString price = ui->Price_2->text().trimmed();
//     // QString dueDate = ui->Due_2->text().trimmed();

//     // if (productName.isEmpty() || price.isEmpty() || dueDate.isEmpty()) {
//     //     QMessageBox::warning(this, "입력 오류", "모든 필드를 입력해주세요.");
//     //     return;
//     // }

//     // // 주문 정보 객체 생성
//     // QJsonObject orderData;
//     // orderData["productName"] = productName;
//     // orderData["price"] = price;
//     // orderData["dueDate"] = dueDate;

//     // // 주문 요청 시그널 발생
//     // emit orderSubmitted(orderData);

//     // // 입력 필드 초기화
//     // ui->ProdName_2->clear();
//     // ui->Price_2->clear();
//     // ui->Due_2->clear();

//     // // 성공 메시지
//     // QMessageBox::information(this, "주문 완료", "주문이 성공적으로 접수되었습니다.");
// }

// void ClientInfoForm::on_pushButton_search_clicked()
// {
//     // 검색 조건 수집
//     QString productName = ui->ProdName->text().trimmed();
//     QString price = ui->Price->text().trimmed();
//     QString dueDate = ui->Due->text().trimmed();

//     // 검색 요청 시그널 발생
//     emit searchOrdersRequested(productName, price, dueDate);
// }

// void ClientInfoForm::on_pushButton_Reset_clicked()
// {
//     // 검색 입력란 초기화
//     ui->ProdName->clear();
//     ui->Price->clear();
//     ui->Due->clear();

//     // 전체 목록 다시 요청
//     emit orderListRequested();
// }

void ClientInfoForm::displayProductList(const QBuffer &buffer)
{
    // 테이블 초기화
    ui->tableWidget->setRowCount(0);

    //통신
    qDebug() << "테이블 디스플레이 :" << buffer.data();
    QByteArray arr = buffer.data();
    arr.remove(0, buffer.pos());
    QJsonArray productArray = QJsonDocument::fromJson(arr).array();
    //통신

    // 테이블 초기화
    ui->tableWidget->setRowCount(0);

    // 제품 목록 표시
    for (int i = 0; i < productArray.size(); ++i) {
        QJsonObject product = productArray[i].toObject();

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // 제품명
        QTableWidgetItem* nameItem = new QTableWidgetItem(product["제품 이름"].toString());
        ui->tableWidget->setItem(row, 0, nameItem);

        // 가격
        QTableWidgetItem* priceItem = new QTableWidgetItem(product["price"].toString());
        ui->tableWidget->setItem(row, 1, priceItem);

        // 납기일
        QTableWidgetItem* dueDateItem = new QTableWidgetItem(product["만료일"].toString());
        ui->tableWidget->setItem(row, 2, dueDateItem);
    }
}

void ClientInfoForm::handleIncomingData()
{
    emit productListRequested();
}
