#ifndef CLIENTINFOFORM_H
#define CLIENTINFOFORM_H

#include <QWidget>

namespace Ui {
class ClientInfoForm;
}

class ClientInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm(QWidget *parent = nullptr);
    ~ClientInfoForm();

private:
    Ui::ClientInfoForm *ui;
};

#endif // CLIENTINFOFORM_H
