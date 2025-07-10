#ifndef ADMININFOFORM_H
#define ADMININFOFORM_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class AdminInfoForm;
}

class AdminInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm(QWidget *parent = nullptr);
    ~AdminInfoForm();

public slots:
    void displayMemberList(const QJsonArray& memberArray);

signals:
    void memberListRequested();
    void searchMembersRequested(const QString& company, const QString& department, const QString& phone);

private slots:
    void on_pushButtonSearch_clicked();

private:
    Ui::AdminInfoForm *ui;
};

#endif // ADMININFOFORM_H
