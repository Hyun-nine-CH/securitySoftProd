#include "admininfoform_ol.h"
#include "ui_admininfoform_ol.h"
#include <QDebug>
#include <QHeaderView>

AdminInfoForm_OL::AdminInfoForm_OL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminInfoForm_OL)
{
    ui->setupUi(this);
    // '검색하기' 버튼의 objectName이 'pushButton_DS'라고 가정
    connect(ui->pushButton_OS, &QPushButton::clicked, this, &AdminInfoForm_OL::on_pushButton_DS_clicked);

    // 테이블 위젯 초기 설정
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"주문번호", "고객사", "제품명", "주문일"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    emit orderListRequested();
}

AdminInfoForm_OL::~AdminInfoForm_OL()
{
    delete ui;
}

void AdminInfoForm_OL::on_pushButton_DS_clicked()
{
    // 스크린샷의 UI 객체 이름(objectName)을 사용
    QString productName = ui->lineEdit_Due->text().trimmed();
    QString dueDate = ui->lineEdit_Price->text().trimmed();

    emit searchOrdersRequested(productName, dueDate);
}

void AdminInfoForm_OL::displayOrderList(const QJsonArray &orderArray)
{
    qDebug() << "Received order list to display:" << orderArray;
    // 테이블 위젯에 데이터를 채우는 로직 구현
}
