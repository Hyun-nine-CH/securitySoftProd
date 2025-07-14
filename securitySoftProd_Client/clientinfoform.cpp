// #include "clientinfoform.h"
// #include "ui_clientinfoform.h"
// #include <QJsonArray>
// #include <QJsonObject>
// #include <QTableWidgetItem>
// #include <QDebug>
// #include <QTimer>
// #include <QMessageBox>
// #include "Protocol.h"

// ClientInfoForm::ClientInfoForm(QWidget *parent) :
//     QWidget(parent),
//     ui(new Ui::ClientInfoForm)
// {
//     ui->setupUi(this);

//     // 테이블 위젯 설정
//     ui->tableWidget->setColumnCount(8); // 회사명, 부서명, 담당자명, 폰 번호, 주소, 구매한 제품, 구매한 가격, 만료일
//     QStringList headers;
//     headers << "회사명" << "부서명" << "담당자명" << "폰 번호" << "주소" << "구매한 제품" << "구매한 가격" << "만료일";
//     ui->tableWidget->setHorizontalHeaderLabels(headers);

//     // 테이블 위젯 열 너비 설정
//     ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//     // 버튼 연결
//     connect(ui->pushButton_order, &QPushButton::clicked, this, &ClientInfoForm::on_pushButton_order_clicked);
//     connect(ui->pushButton_search, &QPushButton::clicked, this, &ClientInfoForm::on_pushButton_search_clicked);
//     connect(ui->pushButton_Reset, &QPushButton::clicked, this, &ClientInfoForm::on_pushButton_Reset_clicked);

//     // 초기 주문 목록 요청 시그널 발생
//     QTimer::singleShot(500, this, [this]() {
//         emit orderListRequested();
//     });
// }

// ClientInfoForm::~ClientInfoForm()
// {
//     delete ui;
// }

// void ClientInfoForm::on_pushButton_order_clicked()
// {
//     // 주문 정보 수집
//     QString productName = ui->ProdName_2->text().trimmed();
//     QString price = ui->Price_2->text().trimmed();
//     QString dueDate = ui->Due_2->text().trimmed();

//     if (productName.isEmpty() || price.isEmpty() || dueDate.isEmpty()) {
//         QMessageBox::warning(this, "입력 오류", "모든 필드를 입력해주세요.");
//         return;
//     }

//     // 주문 정보 객체 생성
//     QJsonObject orderData;
//     orderData["productName"] = productName;
//     orderData["price"] = price;
//     orderData["dueDate"] = dueDate;

//     // 주문 요청 시그널 발생
//     emit orderSubmitted(orderData);

//     // 입력 필드 초기화
//     ui->ProdName_2->clear();
//     ui->Price_2->clear();
//     ui->Due_2->clear();

//     // 성공 메시지
//     QMessageBox::information(this, "주문 완료", "주문이 성공적으로 접수되었습니다.");
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

// void ClientInfoForm::displayOrderList(const QJsonArray& orderArray)
// {
//     // 테이블 초기화
//     ui->tableWidget->setRowCount(0);

//     // 주문 목록 표시
//     for (int i = 0; i < orderArray.size(); ++i) {
//         QJsonObject order = orderArray[i].toObject();

//         int row = ui->tableWidget->rowCount();
//         ui->tableWidget->insertRow(row);

//         // 회사명
//         QTableWidgetItem* companyItem = new QTableWidgetItem(order["RoomId"].toString());
//         ui->tableWidget->setItem(row, 0, companyItem);

//         // 부서명
//         QTableWidgetItem* departmentItem = new QTableWidgetItem(order["department"].toString());
//         ui->tableWidget->setItem(row, 1, departmentItem);

//         // 담당자명
//         QTableWidgetItem* managerItem = new QTableWidgetItem(order["manager"].toString());
//         ui->tableWidget->setItem(row, 2, managerItem);

//         // 폰 번호
//         QTableWidgetItem* phoneItem = new QTableWidgetItem(order["phone"].toString());
//         ui->tableWidget->setItem(row, 3, phoneItem);

//         // 주소
//         QTableWidgetItem* addressItem = new QTableWidgetItem(order["address"].toString());
//         ui->tableWidget->setItem(row, 4, addressItem);

//         // 구매한 제품
//         QTableWidgetItem* productItem = new QTableWidgetItem(order["productName"].toString());
//         ui->tableWidget->setItem(row, 5, productItem);

//         // 구매한 가격
//         QTableWidgetItem* priceItem = new QTableWidgetItem(order["price"].toString());
//         ui->tableWidget->setItem(row, 6, priceItem);

//         // 만료일
//         QTableWidgetItem* dueDateItem = new QTableWidgetItem(order["dueDate"].toString());
//         ui->tableWidget->setItem(row, 7, dueDateItem);
//     }
// }

// void ClientInfoForm::handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename)
// {
//     QJsonDocument doc = QJsonDocument::fromJson(payload);

//     if (dataType == Protocol::Response_Order_List) {
//         if (doc.isArray()) {
//             displayOrderList(doc.array());
//         }
//     }
// }
