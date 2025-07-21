#include "admininfoform_ol.h"
#include "ui_admininfoform_ol.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include "communication.h"

AdminInfoForm_OL::AdminInfoForm_OL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminInfoForm_OL)
{
    ui->setupUi(this);

    // 버튼 연결
    //connect(ui->pushButton_OS, &QPushButton::clicked, this, &AdminInfoForm_OL::on_pushButton_OS_clicked);
    //connect(ui->pushButton_OS_Reset, &QPushButton::clicked, this, &AdminInfoForm_OL::on_pushButton_OS_Reset_clicked);
    //통신클래스 연결
    connect(this, &AdminInfoForm_OL::orderListRequested,Communication::getInstance(),&Communication::RequestAllOrderInfo);
    connect(Communication::getInstance(), &Communication::ReceiveAllOrderInfo,this,&AdminInfoForm_OL::displayOrderList);

}

AdminInfoForm_OL::~AdminInfoForm_OL()
{
    delete ui;
}

QToolBox *AdminInfoForm_OL::getToolBox()
{
    return ui->ClientOrder_Check;
}

void AdminInfoForm_OL::OrderTableSet()
{
    // 테이블 위젯 설정
    ui->tableWidget->setColumnCount(8); // 회사명, 부서명, 담당자명, 폰 번호, 주소, 구매한 제품, 구매한 가격, 만료일
    QStringList headers;
    headers << "회사명" << "부서명" << "담당자명" << "폰 번호" << "주소" << "구매한 제품" << "구매한 가격" << "만료일";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 테이블 위젯 열 너비 설정
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void AdminInfoForm_OL::on_pushButton_OS_clicked()
{
    // 검색 조건 수집
    QString productName = ui->lineEdit_PN->text().trimmed();
    QString price = ui->lineEdit_Price->text().trimmed();
    QString dueDate = ui->lineEdit_Due->text().trimmed();

    // 테이블 위젯의 모든 행을 확인
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        bool match = true;

        // 제품명 검색 조건 확인
        if (!productName.isEmpty()) {
            QString cellProductName = ui->tableWidget->item(row, 5)->text();
            if (!cellProductName.contains(productName, Qt::CaseInsensitive)) {
                match = false;
            }
        }

        // 가격 검색 조건 확인
        if (!price.isEmpty() && match) {
            QString cellPrice = ui->tableWidget->item(row, 6)->text();
            // 쉼표 제거 후 비교 (숫자 형식 처리)
            QString cleanedCellPrice = cellPrice.remove(",");
            QString cleanedPrice = price.remove(",");

            if (!cleanedCellPrice.contains(cleanedPrice)) {
                match = false;
            }
        }

        // 만료일 검색 조건 확인
        if (!dueDate.isEmpty() && match) {
            QString cellDueDate = ui->tableWidget->item(row, 7)->text();
            if (!cellDueDate.contains(dueDate)) {
                match = false;
            }
        }

        // 검색 조건에 맞는 행만 표시
        ui->tableWidget->setRowHidden(row, !match);
    }
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

void AdminInfoForm_OL::displayOrderList(const QBuffer& buffer)
{
    // 테이블 초기화
    ui->tableWidget->setRowCount(0);
    // 가로 스크롤바 활성화
    ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //통신
    qDebug() << "테이블 디스플레이 :" << buffer.data();
    QByteArray arr = buffer.data();
    arr.remove(0, buffer.pos());
    QJsonArray orderArray = QJsonDocument::fromJson(arr).array();
    //통신

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
        QTableWidgetItem* productItem = new QTableWidgetItem(order["productname"].toString());
        ui->tableWidget->setItem(row, 5, productItem);

        // 구매한 가격
        int price = order["price"].toInt();
        QLocale locale(QLocale::Korean);
        QString formattedPrice = locale.toString(price);
        QTableWidgetItem* priceItem = new QTableWidgetItem(formattedPrice);
        ui->tableWidget->setItem(row, 6, priceItem);

        // 만료일
        QTableWidgetItem* dueDateItem = new QTableWidgetItem(order["expire"].toString());
        ui->tableWidget->setItem(row, 7, dueDateItem);
    }
    // 내용에 맞게 칸 크기 조정
    ui->tableWidget->resizeColumnsToContents();
    // 테이블 위젯의 크기 정책을 설정하여 가로 스크롤바가 필요할 때 나타나도록 함
    ui->tableWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // 테이블 헤더 설정
    ui->tableWidget->horizontalHeader()->setStretchLastSection(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

void AdminInfoForm_OL::handleIncomingData()
{
   emit orderListRequested();
}
