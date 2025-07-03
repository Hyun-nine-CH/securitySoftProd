#ifndef ADMININFOFORM_H
#define ADMININFOFORM_H

#include <QWidget>

namespace Ui {
class AdminInfoForm;
}

class AdminInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm(QWidget *parent = nullptr);
    ~AdminInfoForm();

private:
    Ui::AdminInfoForm *ui;
};

#endif // ADMININFOFORM_H
