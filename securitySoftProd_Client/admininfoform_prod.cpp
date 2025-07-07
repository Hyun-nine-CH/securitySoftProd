#include "admininfoform_prod.h"
#include "ui_admininfoform_prod.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QHeaderView>
#include <QCompleter>
#include <QLocale>
#include <QMessageBox>
#include <QLayout>

AdminInfoForm_Prod::AdminInfoForm_Prod(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminInfoForm_Prod)
    , modelMain(new QStandardItemModel(this))
    , modelBefore(new QStandardItemModel(this))
    , modelAfter(new QStandardItemModel(this))
    , modelDeletePreview(new QStandardItemModel(this))

{
    ui->setupUi(this);

    connect(ui->pushButton_search, &QPushButton::clicked, this, &AdminInfoForm_Prod::onSearchClicked);
    connect(ui->pushButton_add,    &QPushButton::clicked, this, &AdminInfoForm_Prod::onAddClicked);
    connect(ui->pushButton_delete, &QPushButton::clicked, this, &AdminInfoForm_Prod::onDeleteClicked);

    connect(ui->pushButton_change, &QPushButton::clicked, this, [=]() {
        if (modelBefore->rowCount() == 0 || modelAfter->rowCount() == 0) return;

        QString id = modelBefore->item(0, 0)->text();

        for (int row = 0; row < modelMain->rowCount(); ++row) {
            if (modelMain->item(row, 0)->text() == id) {
                for (int col = 1; col < modelMain->columnCount(); ++col) {
                    QString newText = modelAfter->item(0, col)->text();
                    modelMain->setItem(row, col, new QStandardItem(newText));
                }
                saveProductsToJson();
                QMessageBox::information(this, "수정 완료", "제품 정보가 변경되었습니다.");
                return;
            }
        }
    });

    // 왼쪽 테이블에서 행 클릭 시 → 오른쪽 before/after 테이블 동기화
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, [=](const QModelIndex& current, const QModelIndex&) {
                // ✅ before 테이블에 선택된 항목 복사
                copySelectedRowToBeforeTable(current);

                // ✅ after 테이블에도 복사하되, 제품 번호는 수정 금지
                int row = current.row();
                modelAfter->clear();
                modelAfter->setHorizontalHeaderLabels({ "제품 번호", "제품 이름", "제품 가격", "제품 만료일" });

                for (int col = 0; col < modelMain->columnCount(); ++col) {
                    QStandardItem* item = new QStandardItem(modelMain->item(row, col)->text());

                    if (col == 0)  // ✨ 제품 번호는 수정 불가
                        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

                    modelAfter->setItem(0, col, item);
                }
            });

    connect(ui->comboBox_Delete, &QComboBox::currentTextChanged, this, [=](const QString& selectedId) {
        modelDeletePreview->clear();  // 해당 테이블뷰에 연결된 QStandardItemModel

        // 헤더 설정
        modelDeletePreview->setHorizontalHeaderLabels(QStringList() << "제품 번호" << "제품 이름" << "제품 가격" << "만료일");

        for (int row = 0; row < modelMain->rowCount(); ++row) {
            if (modelMain->item(row, 0)->text() == selectedId) {
                QList<QStandardItem*> rowItems;
                for (int col = 0; col < modelMain->columnCount(); ++col) {
                    rowItems.append(new QStandardItem(modelMain->item(row, col)->text()));
                }
                modelDeletePreview->appendRow(rowItems);
                break;
            }
        }

        ui->tableView_selectedToDelete->setModel(modelDeletePreview);
    });

    connect(ui->tableView, &QTableView::clicked,
            this, &AdminInfoForm_Prod::copySelectedRowToBeforeTable);

    connect(ui->pushButton_reset, &QPushButton::clicked, this, &AdminInfoForm_Prod::resetTableViewRows);

    QStringList headers = { "제품 번호", "제품 이름", "제품 가격", "제품 만료일" };

    // ✅ Main 전체 제품 목록
    modelMain->setHorizontalHeaderLabels(headers);
    ui->tableView->setModel(modelMain);  // 왼쪽 큰 테이블
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);         // ✨ 부드러운 수평 스크롤
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView->setSortingEnabled(true);

    modelDeletePreview->setHorizontalHeaderLabels(headers);
    ui->tableView_selectedToDelete->setModel(modelDeletePreview);
    ui->tableView_selectedToDelete->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_selectedToDelete->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_selectedToDelete->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView_selectedToDelete->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView_selectedToDelete->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // ✅ 변경 전 테이블
    modelBefore->setHorizontalHeaderLabels(headers);
    ui->tableView_before->setModel(modelBefore);  // 위쪽 변경 전
    ui->tableView_before->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView_before->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView_before->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView_before->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_before->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ✅ 변경 후 테이블
    modelAfter->setHorizontalHeaderLabels(headers);
    ui->tableView_after->setModel(modelAfter);  // 아래쪽 변경 후

    connect(modelAfter, &QStandardItemModel::itemChanged, this, [=](QStandardItem* item){
        int col = item->column();
        QString text = item->text().trimmed();

        if (col == 2) { // 가격
            QString cleaned = text;
            cleaned.remove(',');  // 쉼표 제거
            bool ok;
            int number = cleaned.toInt(&ok);
            if (ok) {
                item->setText(QLocale().toString(number)); // 쉼표 포함 정수로 포맷
            } else {
                QMessageBox::warning(this, "입력 오류", "가격은 숫자만 입력하세요.");
                item->setText(""); // 잘못된 값 초기화
            }
        }

        if (col == 3) { // 만료일
            QString cleaned = text;
            cleaned.remove('-');  // 하이픈 제거
            if (cleaned.length() == 8 && cleaned.toLongLong()) {
                QString formatted = cleaned.left(4) + "-" + cleaned.mid(4, 2) + "-" + cleaned.right(2);
                item->setText(formatted);
            } else {
                QMessageBox::warning(this, "입력 오류", "만료일은 yyyymmdd 형식의 8자리 숫자여야 합니다.");
                item->setText(""); // 잘못된 값 초기화
            }
        }
    });

    ui->tableView_after->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView_after->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView_after->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView_after->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->tableView_after->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ✅ JSON 파일에서 제품 정보 로드
    QFile file("security_software_list.json");  // 실행 디렉토리에 위치해야 함
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            QJsonArray array = doc.array();
            QSet<QString> nameSet;
            QLocale locale(QLocale::Korean);  // 가격 쉼표 포맷

            for (const QJsonValue& val : array) {
                QJsonObject obj = val.toObject();
                QString id     = obj["제품 번호"].toString();
                QString name   = obj["제품 이름"].toString();
                int price      = obj["가격(원)"].toInt();
                QString expiry = obj["만료일"].toString();

                int row = modelMain->rowCount();
                modelMain->setItem(row, 0, new QStandardItem(id));
                modelMain->setItem(row, 1, new QStandardItem(name));
                modelMain->setItem(row, 2, new QStandardItem(locale.toString(price)));  // 쉼표 출력
                modelMain->setItem(row, 3, new QStandardItem(expiry));

                nameSet.insert(name);
            }

            productNameList = nameSet.values();
            productNameList.sort();  // 오름차순 정렬

            // ✅ 검색 페이지 콤보박스
            ui->comboBox_search->setEditable(true);
            ui->comboBox_search->addItems(productNameList);
            ui->comboBox_search->setInsertPolicy(QComboBox::NoInsert);
            ui->comboBox_search->setMaxVisibleItems(8);
            ui->comboBox_search->completer()->setCompletionMode(QCompleter::PopupCompletion);

            productIdList.clear();
            for (int row = 0; row < modelMain->rowCount(); ++row) {
                QString id = modelMain->item(row, 0)->text();
                if (!productIdList.contains(id)) {
                    productIdList.append(id);
                    ui->comboBox_Delete->addItem(id);
                }
            }
        }
    } else {
        qDebug() << "⚠️ security_products.json 파일을 열 수 없습니다.";
    }
}

void AdminInfoForm_Prod::copySelectedRowToBeforeTable(const QModelIndex& index) {
    if (!index.isValid()) return;

    int row = index.row();

    modelBefore->clear();
    modelAfter->clear();

    QStringList headers = { "제품 번호", "제품 이름", "제품 가격", "제품 만료일" };
    modelBefore->setHorizontalHeaderLabels(headers);
    modelAfter->setHorizontalHeaderLabels(headers);

    for (int col = 0; col < modelMain->columnCount(); ++col) {
        QStandardItem* item = modelMain->item(row, col);
        if (item) {
            // ✅ 변경 전 테이블: 포맷 유지된 상태 그대로 복사
            modelBefore->setItem(0, col, new QStandardItem(item->text()));

            // ✅ 변경 후 테이블: 쉼표 및 하이픈 제거 (수정하기 쉽게)
            QString text = item->text();
            if (col == 2) text.remove(',');  // 가격: 쉼표 제거
            if (col == 3) text.remove('-');  // 만료일: 하이픈 제거

            QStandardItem* afterItem = new QStandardItem(text);
            if (col == 0)
                afterItem->setFlags(afterItem->flags() & ~Qt::ItemIsEditable);  // 제품 번호는 수정 금지

            modelAfter->setItem(0, col, afterItem);
        }
    }
}

void AdminInfoForm_Prod::resetTableViewRows() {
    for (int row = 0; row < modelMain->rowCount(); ++row) {
        ui->tableView->setRowHidden(row, false);
    }
}

void AdminInfoForm_Prod::saveProductsToJson()
{
    QJsonArray array;
    QLocale locale(QLocale::Korean);

    for (int row = 0; row < modelMain->rowCount(); ++row) {
        QJsonObject obj;
        obj["제품 번호"] = modelMain->item(row, 0)->text();
        obj["제품 이름"] = modelMain->item(row, 1)->text();
        obj["가격(원)"] = locale.toInt(modelMain->item(row, 2)->text());
        obj["만료일"] = modelMain->item(row, 3)->text();
        array.append(obj);
    }

    QFile file("security_software_list.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(array);
        file.write(doc.toJson());
        file.close();
    }
}

void AdminInfoForm_Prod::onSearchClicked()
{
    QString name = ui->comboBox_search->currentText().trimmed();

    // 가격 검사 및 포맷
    QString priceRaw = ui->lineEdit_searchPrice->text().trimmed();
    QString price;
    if (!priceRaw.isEmpty()) {
        bool ok;
        int priceInt = priceRaw.toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, "입력 오류", "가격은 숫자만 입력해주세요.");
            return;
        }
        price = QLocale().toString(priceInt); // 쉼표 적용
    }

    // 만료일 검사 및 포맷
    QString expiryRaw = ui->lineEdit_searchDue->text().trimmed();
    QString expiry;
    if (!expiryRaw.isEmpty()) {
        if (expiryRaw.length() != 8 || !expiryRaw.toLongLong()) {
            QMessageBox::warning(this, "입력 오류", "만료일은 yyyymmdd 형식의 8자리 숫자여야 합니다.");
            return;
        }
        expiry = expiryRaw.left(4) + "-" + expiryRaw.mid(4, 2) + "-" + expiryRaw.right(2);
    }

    // 검색 수행
    for (int row = 0; row < modelMain->rowCount(); ++row) {
        bool match = false;

        if (!name.isEmpty() && modelMain->item(row, 1)->text().contains(name, Qt::CaseInsensitive))
            match = true;
        if (!price.isEmpty() && modelMain->item(row, 2)->text().contains(price))
            match = true;
        if (!expiry.isEmpty() && modelMain->item(row, 3)->text().contains(expiry))
            match = true;

        ui->tableView->setRowHidden(row, !match);
    }
}

void AdminInfoForm_Prod::onDeleteClicked()
{
    QString selectedId = ui->comboBox_Delete->currentText();

    if (selectedId.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "삭제할 제품 번호를 선택해주세요.");
        return;
    }

    // 삭제 전 확인
    auto reply = QMessageBox::question(this, "삭제 확인", "정말 삭제하시겠습니까?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    // 삭제 수행
    for (int row = 0; row < modelMain->rowCount(); ++row) {
        if (modelMain->item(row, 0)->text() == selectedId) {
            modelMain->removeRow(row);
            ui->comboBox_Delete->removeItem(ui->comboBox_Delete->currentIndex());
            productIdList.removeAll(selectedId);
            saveProductsToJson();
            QMessageBox::information(this, "삭제 완료", "해당 제품이 삭제되었습니다.");
            return;
        }
    }

    QMessageBox::warning(this, "삭제 실패", "해당 제품 번호를 찾을 수 없습니다.");
}

void AdminInfoForm_Prod::onAddClicked()
{
    QString name = ui->lineEdit_addName->text().trimmed();
    QString priceS = ui->lineEdit_addPrice->text().trimmed();
    QString expiryRaw = ui->lineEdit_addDue->text().trimmed();
    if (expiryRaw.length() != 8) {
        QMessageBox::warning(this, "입력 오류", "만료일은 yyyymmdd 형식의 8자리 숫자로 입력해주세요.");
        return;
    }
    QString expiry = expiryRaw.left(4) + "-" + expiryRaw.mid(4,2) + "-" + expiryRaw.right(2);

    if (name.isEmpty() || priceS.isEmpty() || expiry.isEmpty()) return;

    bool ok=false;
    int price = priceS.toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "입력 오류", "가격은 숫자만 입력하세요.");
        return;
    }

    // 🔍 중복 검사 (세 개 항목 모두 일치하는 제품이 있는지)
    for (int row = 0; row < modelMain->rowCount(); ++row) {
        if (modelMain->item(row, 1)->text() == name &&
            modelMain->item(row, 2)->text().remove(",") == priceS &&
            modelMain->item(row, 3)->text() == expiry)
        {
            QMessageBox::warning(this, "중복", "동일한 제품이 이미 존재합니다.");
            return;
        }
    }

    int maxId = 0;
    for (int i = 0; i < modelMain->rowCount(); ++i) {
        int idVal = modelMain->item(i, 0)->text().toInt();
        if (idVal > maxId) maxId = idVal;
    }
    int newId = maxId + 1;
    QString idStr = QString::number(newId);

    QLocale locale(QLocale::Korean);
    int row = modelMain->rowCount();
    modelMain->setItem(row, 0, new QStandardItem(idStr));
    modelMain->setItem(row, 1, new QStandardItem(name));
    modelMain->setItem(row, 2, new QStandardItem(locale.toString(price)));
    modelMain->setItem(row, 3, new QStandardItem(expiry));

    if (!productIdList.contains(idStr)) {
        ui->comboBox_Delete->addItem(idStr);
        productIdList.append(idStr);
    }

    if (!productNameList.contains(name)) {
        ui->comboBox_search->addItem(name); // 제품 이름 콤보박스
        productNameList.append(name);       // 중복 방지를 위한 제품 이름 리스트
    }

    saveProductsToJson();

    ui->lineEdit_addName->clear();
    ui->lineEdit_addPrice->clear();
    ui->lineEdit_addDue->clear();
}

AdminInfoForm_Prod::~AdminInfoForm_Prod()
{
    delete ui;
}
