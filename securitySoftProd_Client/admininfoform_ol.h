#ifndef ADMININFOFORM_OL_H
#define ADMININFOFORM_OL_H

#include <QWidget>

namespace Ui {
class AdminInfoForm_OL;
}

class AdminInfoForm_OL : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm_OL(QWidget *parent = nullptr);
    ~AdminInfoForm_OL();

private:
    Ui::AdminInfoForm_OL *ui;
};

#endif // ADMININFOFORM_OL_H
