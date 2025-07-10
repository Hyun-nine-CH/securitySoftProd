#include "admininfoform.h"
#include "ui_admininfoform.h"
#include <QDebug>
#include <QJsonObject>
#include <QHeaderView>

AdminInfoForm::AdminInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminInfoForm)
{
    ui->setupUi(this);
    // '검색' 버튼의 objectName이 'pushButtonSearch'라고 가정
    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &AdminInfoForm::on_pushButtonSearch_clicked);

    // 테이블 위젯 초기 설정
    ui->tableView->setColumnCount(5);
    ui->tableView->setHorizontalHeaderLabels({"ID", "회사명", "부서명", "담당자", "연락처"});
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    emit memberListRequested();
}

AdminInfoForm::~AdminInfoForm()
{
    delete ui;
}

void AdminInfoForm::on_pushButtonSearch_clicked()
{
    QString company = ui->CompanyName->text().trimmed();
    QString department = ui->DepName->text().trimmed();
    QString phone = ui->PhoneNum->text().trimmed();

    emit searchMembersRequested(company, department, phone);
}

void AdminInfoForm::displayMemberList(const QJsonArray &memberArray)
{
    qDebug() << "Received member list to display:" << memberArray;
    // 테이블 위젯에 데이터를 채우는 로직 구현
}
