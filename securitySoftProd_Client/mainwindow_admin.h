#ifndef MAINWINDOW_ADMIN_H
#define MAINWINDOW_ADMIN_H

#include <QMainWindow>

namespace Ui {
class MainWindow_Admin;
}

class MainWindow_Admin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_Admin(QWidget *parent = nullptr);
    ~MainWindow_Admin();

private:
    Ui::MainWindow_Admin *ui;
};

#endif // MAINWINDOW_ADMIN_H
