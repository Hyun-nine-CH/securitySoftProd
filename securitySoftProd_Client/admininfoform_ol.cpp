#include "admininfoform_ol.h"
#include "ui_admininfoform_ol.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include "Protocol.h"

AdminInfoForm_OL::AdminInfoForm_OL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminInfoForm_OL)
{
    ui->setupUi(this);

    // 테이블 위젯 설정
    ui->tableWidget->setColumnCount(8); // 회사명, 부서명, 담당자명, 폰 번호, 주소, 구매한 제품, 구매한 가격, 만료일
    QStringList headers;
    headers << "회사명" << "부서명" << "담당자명" << "폰 번호" << "주소" << "구매한 제품" << "구매한 가격" << "만료일";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 테이블 위젯 열 너비 설정
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 버튼 연결
    connect(ui->pushButton_OS, &QPushButton::clicked, this, &AdminInfoForm_OL::on_pushButton_OS_clicked);
    connect(ui->pushButton_OS_Reset, &QPushButton::clicked, this, &AdminInfoForm_OL::on_pushButton_OS_Reset_clicked);

    // 초기 주문 목록 요청 시그널 발생
    QTimer::singleShot(500, this, [this]() {
        emit orderListRequested();
    });
}

AdminInfoForm_OL::~AdminInfoForm_OL()
{
    delete ui;
}

void AdminInfoForm_OL::on_pushButton_OS_clicked()
{
    // 검색 조건 수집
    QString productName = ui->lineEdit_PN->text().trimmed();
    QString price = ui->lineEdit_Price->text().trimmed();
    QString dueDate = ui->lineEdit_Due->text().trimmed();

    // 검색 요청 시그널 발생
    emit searchOrdersRequested(productName, price, dueDate);
}

void AdminInfoForm_OL::on_pushButton_OS_Reset_clicked()
{
    // 검색 입력란 초기화
    ui->lineEdit_PN->clear();
    ui->lineEdit_Price->clear();
    ui->lineEdit_Due->clear();

    // 전체 목록 다시 요청
    emit orderListRequested();
}

void AdminInfoForm_OL::displayOrderList(const QJsonArray& orderArray)
{
    // 테이블 초기화
    ui->tableWidget->setRowCount(0);

    // 주문 목록 표시
    for (int i = 0; i < orderArray.size(); ++i) {
        QJsonObject order = orderArray[i].toObject();

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // 회사명
        QTableWidgetItem* companyItem = new QTableWidgetItem(order["RoomId"].toString());
        ui->tableWidget->setItem(row, 0, companyItem);

        // 부서명
        QTableWidgetItem* departmentItem = new QTableWidgetItem(order["department"].toString());
        ui->tableWidget->setItem(row, 1, departmentItem);

        // 담당자명
        QTableWidgetItem* managerItem = new QTableWidgetItem(order["manager"].toString());
        ui->tableWidget->setItem(row, 2, managerItem);

        // 폰 번호
        QTableWidgetItem* phoneItem = new QTableWidgetItem(order["phone"].toString());
        ui->tableWidget->setItem(row, 3, phoneItem);

        // 주소
        QTableWidgetItem* addressItem = new QTableWidgetItem(order["address"].toString());
        ui->tableWidget->setItem(row, 4, addressItem);

        // 구매한 제품
        QTableWidgetItem* productItem = new QTableWidgetItem(order["productName"].toString());
        ui->tableWidget->setItem(row, 5, productItem);

        // 구매한 가격
        QTableWidgetItem* priceItem = new QTableWidgetItem(order["price"].toString());
        ui->tableWidget->setItem(row, 6, priceItem);

        // 만료일
        QTableWidgetItem* dueDateItem = new QTableWidgetItem(order["dueDate"].toString());
        ui->tableWidget->setItem(row, 7, dueDateItem);
    }
}

void AdminInfoForm_OL::handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename)
{
    QJsonDocument doc = QJsonDocument::fromJson(payload);

    if (dataType == Protocol::Response_Order_List) {
        if (doc.isArray()) {
            displayOrderList(doc.array());
        }
    }
}
