// #ifndef CLIENTINFOFORM_PROD_H
// #define CLIENTINFOFORM_PROD_H

// #include <QWidget>
// #include <QJsonArray>
// #include <QBuffer>

// namespace Ui {
// class ClientInfoForm_Prod;
// }

// class ClientInfoForm_Prod : public QWidget
// {
//     Q_OBJECT

// public:
//     explicit ClientInfoForm_Prod(QWidget *parent = nullptr);
//     ~ClientInfoForm_Prod();

//     // 서버에서 받은 데이터 처리
//     void handleIncomingData();
// signals:
//     // 제품 목록 요청 시그널
//     void productListRequested();
//     // 제품 검색 요청 시그널
//     void searchProductsRequested(const QString& name, const QString& price, const QString& dueDate);

// private slots:
//     void on_pushButton_clicked();
//     void on_pushButton_Reset_clicked();
//     // 제품 목록 표시 함수
//     void displayProductList(const QBuffer &buffer);

// private:
//     Ui::ClientInfoForm_Prod *ui;
// };

// #endif // CLIENTINFOFORM_PROD_H
