#ifndef ADMININFOFORM_PROD_H
#define ADMININFOFORM_PROD_H

#include <QWidget>
#include <QJsonArray>

// 전방 선언
class QStandardItemModel;
namespace Ui {
class AdminInfoForm_Prod;
}

class AdminInfoForm_Prod : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm_Prod(QWidget *parent = nullptr);
    ~AdminInfoForm_Prod();

public slots:
    // MainWindow_Admin이 서버로부터 받은 제품 목록을 이 위젯에 표시하기 위해 호출하는 함수
    void displayProductList(const QJsonArray& productArray);

signals:
    // 위젯이 생성될 때 MainWindow_Admin에 제품 목록을 요청하기 위한 시그널
    void productListRequested();
    // 제품 추가/수정/삭제를 요청하는 시그널
    void addProductRequested(const QJsonObject& productData);
    void modifyProductRequested(const QJsonObject& productData);
    void deleteProductRequested(int productId);

private slots:
    void on_pushButton_add_clicked();
    void on_pushButton_change_clicked();
    void on_pushButton_delete_clicked();
    // ... 다른 UI 슬롯들 ...

private:
    Ui::AdminInfoForm_Prod *ui;
    QStandardItemModel* modelMain;
    // ... 다른 멤버 변수들 ...
};

#endif // ADMININFOFORM_PROD_H
