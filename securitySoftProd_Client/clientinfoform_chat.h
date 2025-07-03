#ifndef CLIENTINFOFORM_CHAT_H
#define CLIENTINFOFORM_CHAT_H

#include <QWidget>

namespace Ui {
class ClientInfoForm_Chat;
}

class ClientInfoForm_Chat : public QWidget
{
    Q_OBJECT

public:
    explicit ClientInfoForm_Chat(QWidget *parent = nullptr);
    ~ClientInfoForm_Chat();

private:
    Ui::ClientInfoForm_Chat *ui;
};

#endif // CLIENTINFOFORM_CHAT_H
