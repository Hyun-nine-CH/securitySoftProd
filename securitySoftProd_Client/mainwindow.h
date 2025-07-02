#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QString>
#include <QVector>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *clientModel;

    void loadClientsFromJson(const QString& filePath);
    void displayAllClients();
    void filterClients(const QString &company, const QString &dept, const QString &manager);
    QVector<QJsonObject> allClients;
};
#endif // MAINWINDOW_H
