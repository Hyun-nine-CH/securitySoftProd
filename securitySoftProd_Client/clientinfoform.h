#ifndef CLIENTINFOFORM_H
#define CLIENTINFOFORM_H

#include <QWidget>
#include <QJsonObject> // QJsonObject 사용을 위해 추가

namespace Ui {
class ClientInfoForm;
}

class ClientInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm(QWidget *parent = nullptr);
    ~ClientInfoForm();

signals:
    // '주문하기' 버튼 클릭 시 MainWindow에 주문 정보를 전달하기 위한 시그널
    void orderSubmitted(const QJsonObject& orderData);

private slots:
    // UI의 '주문하기' 버튼(objectName: pushButton_order)과 연결될 슬롯
    void on_pushButton_order_clicked();

private:
    Ui::ClientInfoForm *ui;
};

#endif // CLIENTINFOFORM_H
