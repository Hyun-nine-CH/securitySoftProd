#ifndef ADMININFOFORM_OL_H
#define ADMININFOFORM_OL_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class AdminInfoForm_OL;
}

class AdminInfoForm_OL : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm_OL(QWidget *parent = nullptr);
    ~AdminInfoForm_OL();

    // 주문 목록 표시 함수
    void displayOrderList(const QJsonArray& orderArray);

    // 서버에서 받은 데이터 처리
    void handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename);

signals:
    // 주문 목록 요청 시그널
    void orderListRequested();

    // 주문 검색 요청 시그널
    void searchOrdersRequested(const QString& productName, const QString& price, const QString& dueDate);

private slots:
    void on_pushButton_OS_clicked();
    void on_pushButton_OS_Reset_clicked();

private:
    Ui::AdminInfoForm_OL *ui;
};

#endif // ADMININFOFORM_OL_H
