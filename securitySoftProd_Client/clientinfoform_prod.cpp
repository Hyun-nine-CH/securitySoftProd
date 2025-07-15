#include "clientinfoform_prod.h"
#include "ui_clientinfoform_prod.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include "Protocol.h"
#include "communication.h"

ClientInfoForm_Prod::ClientInfoForm_Prod(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientInfoForm_Prod)
{
    ui->setupUi(this);

    // 테이블 위젯 설정
    ui->tableWidget->setColumnCount(3); // 제품명, 가격, 납기일
    QStringList headers;
    headers << "제품명" << "가격" << "납기일";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 테이블 위젯 열 너비 설정
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 버튼 연결
    connect(ui->pushButton, &QPushButton::clicked, this, &ClientInfoForm_Prod::on_pushButton_clicked);
    connect(ui->pushButton_Reset, &QPushButton::clicked, this, &ClientInfoForm_Prod::on_pushButton_Reset_clicked);

    //통신클래스 연결
    connect(this, &ClientInfoForm_Prod::productListRequested,Communication::getInstance(),&Communication::RequestProductInfo);
    connect(Communication::getInstance(), &Communication::ReceiveProductInfo,this,&ClientInfoForm_Prod::displayProductList);
    // 초기 제품 목록 요청 시그널 발생
    // QTimer::singleShot(500, this, [this]() {
    //     emit productListRequested();
    // });
}

ClientInfoForm_Prod::~ClientInfoForm_Prod()
{
    delete ui;
}

void ClientInfoForm_Prod::on_pushButton_clicked()
{
    // 검색 조건 수집
    QString name = ui->ProdName->text().trimmed();
    QString price = ui->Price->text().trimmed();
    QString dueDate = ui->Due->text().trimmed();

    // 검색 요청 시그널 발생
    emit searchProductsRequested(name, price, dueDate);
}

void ClientInfoForm_Prod::on_pushButton_Reset_clicked()
{
    // 검색 입력란 초기화
    ui->ProdName->clear();
    ui->Price->clear();
    ui->Due->clear();

    // 전체 목록 다시 요청
    emit productListRequested();
}

void ClientInfoForm_Prod::displayProductList(const QBuffer &buffer)
{
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
        QTableWidgetItem* priceItem = new QTableWidgetItem(product["가격(원)"].toString());
        ui->tableWidget->setItem(row, 1, priceItem);

        // 납기일
        QTableWidgetItem* dueDateItem = new QTableWidgetItem(product["만료일"].toString());
        ui->tableWidget->setItem(row, 2, dueDateItem);
    }
}

void ClientInfoForm_Prod::handleIncomingData()
{
    emit productListRequested();
}
