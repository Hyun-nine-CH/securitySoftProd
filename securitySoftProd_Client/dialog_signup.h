#ifndef DIALOG_SIGNUP_H
#define DIALOG_SIGNUP_H

#include <QDialog>

namespace Ui {
class Dialog_SignUp;
}

class Dialog_SignUp : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_SignUp(QWidget *parent = nullptr);
    ~Dialog_SignUp();

private:
    Ui::Dialog_SignUp *ui;
};

#endif // DIALOG_SIGNUP_H
