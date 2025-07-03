#ifndef CLIENTINFOFORM_PROD_H
#define CLIENTINFOFORM_PROD_H

#include <QWidget>

namespace Ui {
class ClientInfoForm_Prod;
}

class ClientInfoForm_Prod : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm_Prod(QWidget *parent = nullptr);
    ~ClientInfoForm_Prod();

private:
    Ui::ClientInfoForm_Prod *ui;
};

#endif // CLIENTINFOFORM_PROD_H
