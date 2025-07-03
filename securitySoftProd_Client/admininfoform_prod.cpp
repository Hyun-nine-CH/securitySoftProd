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
{
    ui->setupUi(this);

    connect(ui->pushButton_search, &QPushButton::clicked, this, &AdminInfoForm_Prod::onSearchClicked);
    connect(ui->pushButton_add,    &QPushButton::clicked, this, &AdminInfoForm_Prod::onAddClicked);
    connect(ui->pushButton_delete, &QPushButton::clicked, this, &AdminInfoForm_Prod::onDeleteClicked);
    connect(ui->pushButton_change, &QPushButton::clicked, this, [=]() { /* 변경 처리 람다 */ });

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, [=](const QModelIndex &current, const QModelIndex &) {
                if (!current.isValid()) return;

                int row = current.row();

                modelBefore->removeRows(0, modelBefore->rowCount());
                modelAfter->removeRows(0, modelAfter->rowCount());

                for (int col = 0; col < modelMain->columnCount(); ++col) {
                    QStandardItem *itemBefore = new QStandardItem(modelMain->item(row, col)->text());
                    QStandardItem *itemAfter  = new QStandardItem(modelMain->item(row, col)->text());

                    modelBefore->setItem(0, col, itemBefore);
                    modelAfter->setItem(0, col, itemAfter);
                }
            });

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

    connect(ui->pushButton_reset, &QPushButton::clicked, this, &AdminInfoForm_Prod::resetTableViewRows);

    QStringList headers = { "제품 번호", "제품 이름", "제품 가격", "제품 만료일" };

    // ✅ Main 전체 제품 목록
    modelMain->setHorizontalHeaderLabels(headers);
    ui->tableView->setModel(modelMain);  // 왼쪽 큰 테이블
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ✅ 변경 전 테이블
    modelBefore->setHorizontalHeaderLabels(headers);
    ui->tableView_before->setModel(modelBefore);  // 위쪽 변경 전
    ui->tableView_before->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ✅ 변경 후 테이블
    modelAfter->setHorizontalHeaderLabels(headers);
    ui->tableView_after->setModel(modelAfter);  // 아래쪽 변경 후
    ui->tableView_after->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_after->setEditTriggers(QAbstractItemView::AllEditTriggers); // ✅ 사용자 직접 수정 가능

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
            ui->comboBox_change->setEditable(true);
            ui->comboBox_change->addItems(productNameList);
            ui->comboBox_change->setInsertPolicy(QComboBox::NoInsert);
            ui->comboBox_change->setMaxVisibleItems(8);
            ui->comboBox_change->completer()->setCompletionMode(QCompleter::PopupCompletion);

            // ✅ 삭제 페이지 콤보박스 (추가)
            ui->comboBox_delete->setEditable(true);
            ui->comboBox_delete->addItems(productNameList);
            ui->comboBox_delete->setInsertPolicy(QComboBox::NoInsert);
            ui->comboBox_delete->setMaxVisibleItems(8);
            ui->comboBox_delete->completer()->setCompletionMode(QCompleter::PopupCompletion);

            // ✅ 검색 페이지 콤보박스
            ui->comboBox_search->setEditable(true);
            ui->comboBox_search->addItems(productNameList);
            ui->comboBox_search->setInsertPolicy(QComboBox::NoInsert);
            ui->comboBox_search->setMaxVisibleItems(8);
            ui->comboBox_search->completer()->setCompletionMode(QCompleter::PopupCompletion);
        }
    } else {
        qDebug() << "⚠️ security_products.json 파일을 열 수 없습니다.";
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

    QFile file("security_products.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(array);
        file.write(doc.toJson());
        file.close();
    }
}

void AdminInfoForm_Prod::onSearchClicked()
{
    QString name   = ui->comboBox_search->currentText().trimmed();
    QString price  = ui->lineEdit_searchPrice->text().trimmed();
    QString expiry = ui->lineEdit_searchDue->text().trimmed();

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
    QString name = ui->comboBox_delete->currentText().trimmed();
    QString priceS = ui->lineEdit_deletePrice->text().trimmed();
    QString expiry = ui->lineEdit_deleteDue->text().trimmed();

    if (name.isEmpty() || priceS.isEmpty() || expiry.isEmpty()) return;

    auto reply = QMessageBox::question(this, "삭제 확인", "정말 삭제하시겠습니까?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    for (int row = 0; row < modelMain->rowCount(); ++row) {
        if (modelMain->item(row, 1)->text() == name &&
            modelMain->item(row, 2)->text().remove(",") == priceS &&
            modelMain->item(row, 3)->text() == expiry)
        {
            modelMain->removeRow(row);
            saveProductsToJson();
            QMessageBox::information(this, "삭제 완료", "해당 제품이 삭제되었습니다.");
            return;
        }
    }

    QMessageBox::warning(this, "삭제 실패", "일치하는 제품이 존재하지 않습니다.");
}

void AdminInfoForm_Prod::onAddClicked()
{
    QString name = ui->lineEdit_addName->text().trimmed();
    QString priceS = ui->lineEdit_addPrice->text().trimmed();
    QString expiry = ui->lineEdit_addDue->text().trimmed();

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
    QString idStr = QString("%1").arg(newId, 2, 10, QLatin1Char('0'));

    QLocale locale(QLocale::Korean);
    int row = modelMain->rowCount();
    modelMain->setItem(row, 0, new QStandardItem(idStr));
    modelMain->setItem(row, 1, new QStandardItem(name));
    modelMain->setItem(row, 2, new QStandardItem(locale.toString(price)));
    modelMain->setItem(row, 3, new QStandardItem(expiry));

    if (!productNameList.contains(name)) {
        ui->comboBox_change->addItem(name);
        ui->comboBox_delete->addItem(name);
        ui->comboBox_search->addItem(name);
        productNameList.append(name);
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
