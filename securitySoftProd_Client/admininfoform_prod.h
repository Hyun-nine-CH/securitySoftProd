#ifndef ADMININFOFORM_PROD_H
#define ADMININFOFORM_PROD_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>
#include <QBuffer>
#include <QToolBox>
namespace Ui {
class AdminInfoForm_Prod;
}

class AdminInfoForm_Prod : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm_Prod(QWidget *parent = nullptr);
    ~AdminInfoForm_Prod();

    QToolBox* getToolBox();

    // 제품 목록 표시 함수
    void displayProductList(const QBuffer &buffer);
    void handleIncomingData();
    void ProductTableSet();

signals:
    //제품 목록 요청 시그널
    void productListRequested();

    // 제품 검색 요청 시그널
    //void searchProductsRequested(const QString& name, const QString& price, const QString& dueDate);

    // 제품 추가 요청 시그널
    void addProductRequested(const QJsonObject& productData);

    // 제품 삭제 요청 시그널
    void deleteProductRequested(const QJsonObject& productData);

    // 제품 변경 요청 시그널
    void updateProductRequested(const QJsonObject& productData);

private slots:
    void on_pushButton_search_clicked();
    void on_pushButton_reset_clicked();
    void on_pushButton_add_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_update_clicked();
    void on_comboBox_Delete_currentIndexChanged(int index);
    void on_tableWidget_itemClicked(QTableWidgetItem* item);


private:
    Ui::AdminInfoForm_Prod *ui;
};

#endif // ADMININFOFORM_PROD_H
