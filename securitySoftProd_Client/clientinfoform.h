// #ifndef CLIENTINFOFORM_H
// #define CLIENTINFOFORM_H

// #include <QWidget>
// #include <QJsonArray>
// #include <QJsonObject>

// namespace Ui {
// class ClientInfoForm;
// }

// class ClientInfoForm : public QWidget
// {
//     Q_OBJECT

// public:
//     explicit ClientInfoForm(QWidget *parent = nullptr);
//     ~ClientInfoForm();

//     // 주문 목록 표시 함수
//     void displayOrderList(const QJsonArray& orderArray);

//     // 서버에서 받은 데이터 처리
//     void handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename);

// signals:
//     // 주문 제출 시그널
//     void orderSubmitted(const QJsonObject& orderData);

//     // 주문 목록 요청 시그널
//     void orderListRequested();

//     // 주문 검색 요청 시그널
//     void searchOrdersRequested(const QString& productName, const QString& price, const QString& dueDate);

// private slots:
//     void on_pushButton_order_clicked();
//     void on_pushButton_search_clicked();
//     void on_pushButton_Reset_clicked();

// private:
//     Ui::ClientInfoForm *ui;
// };

// #endif // CLIENTINFOFORM_H
