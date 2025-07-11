#ifndef CLIENTINFOFORM_PROD_H
#define CLIENTINFOFORM_PROD_H

#include <QWidget>
#include <QJsonArray>
#include <QStandardItemModel>

namespace Ui {
class ClientInfoForm_Prod;
}

class ClientInfoForm_Prod : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm_Prod(QWidget *parent = nullptr);
    ~ClientInfoForm_Prod();

public slots:
    void displayProductList(const QJsonArray& productArray);

signals:
    void productListRequested();
    void searchProductsRequested(const QString& name, const QString& price, const QString& dueDate);

private slots:
    // UI의 '검색하기' 버튼 (objectName: pushButton)과 연결될 슬롯
    void on_pushButton_clicked();

private:
    Ui::ClientInfoForm_Prod *ui;
    QStandardItemModel* model;
};

#endif // CLIENTINFOFORM_PROD_H
