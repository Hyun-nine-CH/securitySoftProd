#include "admininfoform_prod.h"
#include "ui_admininfoform_prod.h"
#include <QStandardItemModel>
#include <QJsonObject>
#include <QHeaderView>
#include <QMessageBox>

AdminInfoForm_Prod::AdminInfoForm_Prod(QWidget *parent)
    : QWidget(parent), ui(new Ui::AdminInfoForm_Prod)
{
    ui->setupUi(this);
    // modelMain = new QStandardItemModel(this);
    // ui->tableWidget->setModel(modelMain);

    // 헤더 미리 설정해도 좋아
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"제품 번호", "제품 이름", "가격(원)", "만료일"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    emit productListRequested();
}

AdminInfoForm_Prod::~AdminInfoForm_Prod()
{
    delete ui;
}

// 서버로부터 받은 제품 목록을 테이블에 표시
void AdminInfoForm_Prod::displayProductList(const QJsonArray &productArray)
{
    ui->tableWidget->clear(); // 기존 내용 초기화
    ui->tableWidget->setRowCount(productArray.size()); // 행 개수 설정
    ui->tableWidget->setColumnCount(4); // 열 개수 고정

    // 헤더 고정
    QStringList headers = {"제품 번호", "제품 이름", "가격(원)", "만료일"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < productArray.size(); ++row) {
        QJsonObject obj = productArray.at(row).toObject();

        // 각 셀에 아이템 설정
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(obj["Id"].toInt())));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(obj["Name"].toString()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(obj["Price"].toInt())));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(obj["ExpiryDate"].toString()));
    }

    // 헤더 크기 자동 조절 (선택사항)
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

// '추가' 버튼 클릭 시
void AdminInfoForm_Prod::on_pushButton_add_clicked()
{
    // UI에서 입력값 가져오기
    QJsonObject newProduct;
    newProduct["Name"] = ui->lineEdit_addName->text();
    newProduct["Price"] = ui->lineEdit_addPrice->text().toInt();
    newProduct["ExpiryDate"] = ui->lineEdit_addDue->text();

    // MainWindow_Admin에 제품 추가 요청 시그널 전송
    emit addProductRequested(newProduct);
}

// '수정' 버튼 클릭 시
void AdminInfoForm_Prod::on_pushButton_change_clicked()
{
    // 수정할 제품 정보를 QJsonObject로 구성
    // ...
    // emit modifyProductRequested(modifiedProduct);
}

// '삭제' 버튼 클릭 시
void AdminInfoForm_Prod::on_pushButton_delete_clicked()
{
    // 삭제할 제품 ID 가져오기
    // ...
    // emit deleteProductRequested(productId);
}
