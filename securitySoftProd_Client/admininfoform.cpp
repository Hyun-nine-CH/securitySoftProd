#include "admininfoform.h"
#include "ui_admininfoform.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include "communication.h"

AdminInfoForm::AdminInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminInfoForm)
{
    ui->setupUi(this);

    ui->tableWidget->clear();

    //통신클래스 연결
    connect(this, &AdminInfoForm::clientListRequested,Communication::getInstance(),&Communication::RequestUserInfo);
    connect(Communication::getInstance(), &Communication::ReceiveUserInfo,this,&AdminInfoForm::displayClientList);

    // 버튼 연결
    //connect(ui->pushButtonSearch, &QPushButton::clicked, this, &AdminInfoForm::on_pushButtonSearch_clicked);
    //connect(ui->pushButtonReset, &QPushButton::clicked, this, &AdminInfoForm::on_pushButtonReset_clicked);

}

AdminInfoForm::~AdminInfoForm()
{
    delete ui;
}

void AdminInfoForm::on_pushButtonSearch_clicked()
{
    // 검색 조건 수집
    QString company = ui->CompanyName->text().trimmed();
    QString department = ui->DepName->text().trimmed();
    QString manager = ui->ClientName->text().trimmed();
    QString phone = ui->PhoneNum->text().trimmed();

    // 검색 요청 시그널 발생
    emit searchClientsRequested(company, department, manager, phone);
}

void AdminInfoForm::on_pushButtonReset_clicked()
{
    // 검색 입력란 초기화
    ui->CompanyName->clear();
    ui->DepName->clear();
    ui->ClientName->clear();
    ui->PhoneNum->clear();

    // 전체 목록 다시 요청
    emit clientListRequested();
}

void AdminInfoForm::displayClientList(const QBuffer &buffer)
{
    // 테이블 초기화
    ui->tableWidget->setRowCount(0);

    //통신
    qDebug() << "테이블 디스플레이 :" << buffer.data();
    QByteArray arr = buffer.data();
    arr.remove(0, buffer.pos());
    QJsonArray clientArray = QJsonDocument::fromJson(arr).array();
    //통신

    // 클라이언트 목록 표시
    for (int i = 0; i < clientArray.size(); ++i) {
        QJsonObject client = clientArray[i].toObject();

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // 회사명
        QTableWidgetItem* roomIdItem = new QTableWidgetItem(client["RoomId"].toString());
        ui->tableWidget->setItem(row, 0, roomIdItem);

        // 부서명
        QTableWidgetItem* departmentItem = new QTableWidgetItem(client["department"].toString());
        ui->tableWidget->setItem(row, 1, departmentItem);

        // 담당자명
        QTableWidgetItem* managerItem = new QTableWidgetItem(client["manager"].toString());
        ui->tableWidget->setItem(row, 2, managerItem);

        // 주소
        QTableWidgetItem* addressItem = new QTableWidgetItem(client["address"].toString());
        ui->tableWidget->setItem(row, 3, addressItem);
    }
}

QToolBox *AdminInfoForm::getToolBox()
{
    return ui->toolBox;
}

void AdminInfoForm::UserTableSet()
{
    // 테이블 위젯 설정
    ui->tableWidget->setColumnCount(4); // 회사명, 부서명, 담당자명, 주소
    QStringList headers;
    headers << "회사명" << "부서명" << "담당자명" << "주소";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 테이블 위젯 열 너비 설정
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void AdminInfoForm::handleIncomingData()
{
    emit clientListRequested();
}
