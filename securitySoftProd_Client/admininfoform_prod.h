#ifndef ADMININFOFORM_PROD_H
#define ADMININFOFORM_PROD_H

#include <QWidget>
#include <QStandardItemModel>
#include <QStringList>

namespace Ui {
class AdminInfoForm_Prod;
}

class AdminInfoForm_Prod : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm_Prod(QWidget *parent = nullptr);
    ~AdminInfoForm_Prod();

private:
    Ui::AdminInfoForm_Prod *ui;

    QStandardItemModel* modelMain;
    QStandardItemModel* modelBefore;
    QStandardItemModel* modelAfter;

    QStringList productNameList;

    void resetTableViewRows();
    void onSearchClicked();      // 검색 기능
    void onAddClicked();         // 추가 기능
    void onDeleteClicked();      // 삭제 기능
    void saveProductsToJson();   // JSON 저장 기능
    void reassignIds();          // ID 자동 재정렬 기능
};

#endif // ADMININFOFORM_PROD_H

