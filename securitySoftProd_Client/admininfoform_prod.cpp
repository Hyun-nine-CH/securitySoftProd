#include "admininfoform_prod.h"
#include "ui_admininfoform_prod.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QLocale>
#include <QDate>
#include "Protocol.h"
#include "communication.h"

AdminInfoForm_Prod::AdminInfoForm_Prod(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminInfoForm_Prod)
{
    ui->setupUi(this);
    QStringList headers;
    ui->tableWidget->clear();
    // 테이블 위젯 열 너비 설정
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 삭제 테이블 위젯 설정
    ui->tableWidget_delete->setColumnCount(4);
    ui->tableWidget_delete->setHorizontalHeaderLabels(headers);
    ui->tableWidget_delete->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 변경 전/후 테이블 위젯 설정
    ui->tableWidget_before->setColumnCount(4);
    ui->tableWidget_before->setHorizontalHeaderLabels(headers);
    ui->tableWidget_before->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget_after->setColumnCount(4);
    ui->tableWidget_after->setHorizontalHeaderLabels(headers);
    ui->tableWidget_after->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 변경 후 테이블은 제품 번호를 제외하고 편집 가능하도록 설정
    ui->tableWidget_after->setEditTriggers(QAbstractItemView::DoubleClicked);

    //버튼
    connect(ui->pushButton_change, &QPushButton::clicked, this, &AdminInfoForm_Prod::on_pushButton_update_clicked);
    //통신클래스 연결
    connect(this, &AdminInfoForm_Prod::productListRequested,Communication::getInstance(),&Communication::RequestProductInfo);
    connect(Communication::getInstance(), &Communication::ReceiveProductInfo_ad,this,&AdminInfoForm_Prod::displayProductList);

    //추가
    connect(this, &AdminInfoForm_Prod::addProductRequested,Communication::getInstance(),&Communication::RequestProductAdd);
    //삭제
    connect(this, &AdminInfoForm_Prod::deleteProductRequested,Communication::getInstance(),&Communication::RequestProductDel);
    //수정
    connect(this, &AdminInfoForm_Prod::updateProductRequested,Communication::getInstance(),&Communication::RequestProductMod);
}

AdminInfoForm_Prod::~AdminInfoForm_Prod()
{
    delete ui;
}

QToolBox* AdminInfoForm_Prod::getToolBox()
{
    return ui->toolBox;
}

// void AdminInfoForm_Prod::on_pushButton_search_clicked()
// {
//     // 검색 조건 수집
//     QString name = ui->comboBox_search->currentText().trimmed();
//     QString price = ui->lineEdit_searchPrice->text().trimmed();
//     QString dueDate = ui->lineEdit_searchDue->text().trimmed();

//     // 검색 요청 시그널 발생
//     emit searchProductsRequested(name, price, dueDate);
// }

// void AdminInfoForm_Prod::on_pushButton_reset_clicked()
// {
//     // 검색 입력란 초기화
//     ui->comboBox_search->setCurrentText("");
//     ui->lineEdit_searchPrice->clear();
//     ui->lineEdit_searchDue->clear();

//     // 전체 목록 다시 요청
//     emit productListRequested();
// }

void AdminInfoForm_Prod::on_pushButton_add_clicked()
{
    // 추가 정보 수집
    QString name = ui->lineEdit_addName->text().trimmed();
    QString price = ui->lineEdit_addPrice->text().trimmed();
    QString dueDate = ui->lineEdit_addDue->text().trimmed();

    // 유효성 검사
    if (name.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "제품 이름을 입력해주세요.");
        return;
    }

    bool priceOk;
    int priceValue = price.toInt(&priceOk);
    if (!priceOk || priceValue <= 0) {
        QMessageBox::warning(this, "입력 오류", "가격은 양의 정수로 입력해주세요.");
        return;
    }

    if (dueDate.length() != 8 || !dueDate.toInt(&priceOk)) {
        QMessageBox::warning(this, "입력 오류", "만료일은 8자리 숫자(YYYYMMDD)로 입력해주세요.");
        return;
    }

    // 만료일 형식 변환 (YYYYMMDD -> YYYY-MM-DD)
    QString formattedDueDate = dueDate.mid(0, 4) + "-" + dueDate.mid(4, 2) + "-" + dueDate.mid(6, 2);

    // 제품 추가 요청 객체 생성
    QJsonObject addData;
    addData["productname"] = name;
    addData["price"] = priceValue;
    addData["expire"] = formattedDueDate;

    // 제품 추가 요청 시그널 발생
    emit addProductRequested(addData);

    // 입력 필드 초기화
    ui->lineEdit_addName->clear();
    ui->lineEdit_addPrice->clear();
    ui->lineEdit_addDue->clear();
}
void AdminInfoForm_Prod::on_comboBox_Delete_currentIndexChanged(int index)
{
    QString productId = ui->comboBox_Delete->currentText();
    if (productId.isEmpty()) return;

    // 선택된 제품 ID에 해당하는 제품 정보 찾기
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        if (ui->tableWidget->item(i, 0)->text() == productId) {
            // 삭제 테이블 위젯 초기화
            ui->tableWidget_delete->setRowCount(0);
            ui->tableWidget_delete->insertRow(0);

            // 선택된 제품 정보를 삭제 테이블에 표시
            for (int j = 0; j < 4; j++) {
                QTableWidgetItem* item = new QTableWidgetItem(ui->tableWidget->item(i, j)->text());
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 읽기 전용
                ui->tableWidget_delete->setItem(0, j, item);
            }
            break;
        }
    }
}

void AdminInfoForm_Prod::on_pushButton_delete_clicked()
{
    QString productId = ui->comboBox_Delete->currentText();
    if (productId.isEmpty()) {
        QMessageBox::warning(this, "삭제 오류", "삭제할 제품을 선택해주세요.");
        return;
    }

    // 삭제 확인 메시지
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "제품 삭제", "정말로 이 제품을 삭제하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 제품 삭제 요청 객체 생성
        QJsonObject deleteData;
        deleteData["number"] = productId;

        // 제품 삭제 요청 시그널 발생
        emit deleteProductRequested(deleteData);

        // comboBox 초기화
        ui->comboBox_Delete->setCurrentText("");
        ui->tableWidget_delete->setRowCount(0);
    }
}

void AdminInfoForm_Prod::on_tableWidget_itemClicked(QTableWidgetItem* item)
{
    int row = item->row();

    // 변경 전 테이블 위젯 초기화
    ui->tableWidget_before->setRowCount(0);
    ui->tableWidget_before->insertRow(0);

    // 변경 후 테이블 위젯 초기화
    ui->tableWidget_after->setRowCount(0);
    ui->tableWidget_after->insertRow(0);

    // 선택된 제품 정보를 변경 전/후 테이블에 표시
    for (int j = 0; j < 4; j++) {
        // 변경 전 테이블 (읽기 전용)
        QTableWidgetItem* beforeItem = new QTableWidgetItem(ui->tableWidget->item(row, j)->text());
        beforeItem->setFlags(beforeItem->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_before->setItem(0, j, beforeItem);

        // 변경 후 테이블 (제품 번호는 읽기 전용, 나머지는 편집 가능)
        QTableWidgetItem* afterItem = new QTableWidgetItem(ui->tableWidget->item(row, j)->text());
        if (j == 0) { // 제품 번호는 편집 불가
            afterItem->setFlags(afterItem->flags() & ~Qt::ItemIsEditable);
        }
        ui->tableWidget_after->setItem(0, j, afterItem);
    }
}

void AdminInfoForm_Prod::on_pushButton_update_clicked()
{
    if (ui->tableWidget_after->rowCount() == 0) {
        QMessageBox::warning(this, "변경 오류", "변경할 제품을 선택해주세요.");
        return;
    }

    // 변경 정보 수집
    QString productId = ui->tableWidget_after->item(0, 0)->text();
    QString name = ui->tableWidget_after->item(0, 1)->text();
    QString priceText = ui->tableWidget_after->item(0, 2)->text().replace(",", ""); // 쉼표 제거
    QString dueDateText = ui->tableWidget_after->item(0, 3)->text();

    // 유효성 검사
    if (name.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "제품 이름을 입력해주세요.");
        return;
    }

    bool priceOk;
    int priceValue = priceText.toInt(&priceOk);
    if (!priceOk || priceValue <= 0) {
        QMessageBox::warning(this, "입력 오류", "가격은 양의 정수로 입력해주세요.");
        return;
    }

    // 만료일 형식 검사 (YYYY-MM-DD)
    QDate dueDate = QDate::fromString(dueDateText, "yyyy-MM-dd");
    if (!dueDate.isValid()) {
        QMessageBox::warning(this, "입력 오류", "만료일은 YYYY-MM-DD 형식으로 입력해주세요.");
        return;
    }

    // 제품 변경 요청 객체 생성
    QJsonObject updateData;
    updateData["number"] = productId;
    updateData["productname"] = name;
    updateData["price"] = priceValue;
    updateData["expire"] = dueDateText;

    // 제품 변경 요청 시그널 발생
    emit updateProductRequested(updateData);

    // 변경 테이블 초기화
    ui->tableWidget_before->setRowCount(0);
    ui->tableWidget_after->setRowCount(0);
}

void AdminInfoForm_Prod::displayProductList(const QBuffer &buffer)
{
    // 테이블 초기화
    ui->tableWidget->setRowCount(0);

    // comboBox 초기화
    ui->comboBox_search->clear();
    ui->comboBox_Delete->clear();

    //통신
    qDebug() << "테이블 디스플레이 :" << buffer.data();
    QByteArray arr = buffer.data();
    arr.remove(0, buffer.pos());
    QJsonArray productArray = QJsonDocument::fromJson(arr).array();
    //통신

    // 제품 목록 표시
    for (int i = 0; i < productArray.size(); ++i) {
        QJsonObject product = productArray[i].toObject();

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // 제품 번호
        QString productId = product["number"].toString();
        QTableWidgetItem* idItem = new QTableWidgetItem(productId);
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable); // 읽기 전용
        ui->tableWidget->setItem(row, 0, idItem);

        // 제품 이름
        QString productName = product["productname"].toString();
        QTableWidgetItem* nameItem = new QTableWidgetItem(productName);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable); // 읽기 전용
        ui->tableWidget->setItem(row, 1, nameItem);

        // 가격 (3자리 쉼표 형식으로 표시)
        int price = product["price"].toInt();
        QLocale locale(QLocale::Korean);
        QString formattedPrice = locale.toString(price);
        QTableWidgetItem* priceItem = new QTableWidgetItem(formattedPrice);
        priceItem->setFlags(priceItem->flags() & ~Qt::ItemIsEditable); // 읽기 전용
        ui->tableWidget->setItem(row, 2, priceItem);

        // 만료일
        QString dueDate = product["expire"].toString();
        QTableWidgetItem* dueDateItem = new QTableWidgetItem(dueDate);
        dueDateItem->setFlags(dueDateItem->flags() & ~Qt::ItemIsEditable); // 읽기 전용
        ui->tableWidget->setItem(row, 3, dueDateItem);

        // comboBox에 제품 이름과 번호 추가
        ui->comboBox_search->addItem(productName);
        ui->comboBox_Delete->addItem(productId);
    }

    // 테이블 정렬
    ui->tableWidget->sortItems(0, Qt::AscendingOrder);
}

void AdminInfoForm_Prod::ProductTableSet()
{
    // 메인 테이블 위젯 설정
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(4); // 제품 번호, 제품 이름, 가격, 만료일
    QStringList headers;
    headers << "제품 번호" << "제품 이름" << "가격(원)" << "만료일";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

void AdminInfoForm_Prod::handleIncomingData()
{
    emit productListRequested();
}
