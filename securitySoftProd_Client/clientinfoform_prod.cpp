#include "clientinfoform_prod.h"
#include "ui_clientinfoform_prod.h"
#include <QDebug>
#include <QHeaderView>
#include <QJsonObject>
#include <QStandardItemModel>

ClientInfoForm_Prod::ClientInfoForm_Prod(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientInfoForm_Prod),
    model(new QStandardItemModel(this)) // 모델 생성
{
    ui->setupUi(this);

    // 검색 버튼 연결
    connect(ui->pushButton, &QPushButton::clicked, this, &ClientInfoForm_Prod::on_pushButton_clicked);

    // 테이블뷰에 모델 설정
    model->setColumnCount(3);
    model->setHorizontalHeaderLabels({"제품명", "가격", "만료일"});
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 시작 시 전체 제품 목록 요청
    emit productListRequested();
}

ClientInfoForm_Prod::~ClientInfoForm_Prod()
{
    delete ui;
}

void ClientInfoForm_Prod::on_pushButton_clicked()
{
    QString name = ui->ProdName->text().trimmed();
    QString price = ui->Price->text().trimmed();
    QString dueDate = ui->Due->text().trimmed();

    qDebug() << "Searching products with criteria:" << name << price << dueDate;

    emit searchProductsRequested(name, price, dueDate);
}

void ClientInfoForm_Prod::displayProductList(const QJsonArray &productArray)
{
    qDebug() << "Received product list to display:" << productArray;

    model->removeRows(0, model->rowCount()); // 기존 행 삭제

    for (const QJsonValue &value : productArray) {
        QJsonObject obj = value.toObject();

        QList<QStandardItem*> rowItems;
        rowItems << new QStandardItem(obj["Name"].toString());
        rowItems << new QStandardItem(QString::number(obj["Price"].toInt()));
        rowItems << new QStandardItem(obj["ExpiryDate"].toString());

        model->appendRow(rowItems);
    }
}

