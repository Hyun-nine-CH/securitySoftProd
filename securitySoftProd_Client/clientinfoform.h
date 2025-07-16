#ifndef CLIENTINFOFORM_H
#define CLIENTINFOFORM_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QBuffer>
namespace Ui {
class ClientInfoForm;
}

class ClientInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm(QWidget *parent = nullptr);
    ~ClientInfoForm();

    // 서버에서 받은 데이터 처리
    void handleIncomingData();

signals:
    // 주문 제출 시그널
    //void orderSubmitted(const QJsonObject& orderData);

    // 주문 목록 요청 시그널
    void orderListRequested();

    // 주문 검색 요청 시그널
    //void searchOrdersRequested(const QString& productName, const QString& price, const QString& dueDate);
    // 제품 목록 요청 시그널
    void productListRequested();
private slots:
    //void on_pushButton_order_clicked();
    //void on_pushButton_search_clicked();
    //void on_pushButton_Reset_clicked();
    void displayProductList(const QBuffer &buffer);
    void displayOrderList(const QBuffer &buffer);
    void ProductTableSet();
    void OrderTableSet();
    void ChangeTool(int index);

private:
    Ui::ClientInfoForm *ui;
};

#endif // CLIENTINFOFORM_H
