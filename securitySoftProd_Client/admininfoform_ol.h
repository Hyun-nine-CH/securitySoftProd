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

public slots:
    void displayOrderList(const QJsonArray& orderArray);

signals:
    void orderListRequested();
    void searchOrdersRequested(const QString& productName, const QString& dueDate);

private slots:
    void on_pushButton_DS_clicked();

private:
    Ui::AdminInfoForm_OL *ui;
};

#endif // ADMININFOFORM_OL_H
