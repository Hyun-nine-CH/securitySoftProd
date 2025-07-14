#include "dialog_log.h"
#include "mainwindow_admin.h"
#include "mainwindow.h"
#include "communication.h"

#include <QApplication>
#include <QJsonObject>
#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Dialog_log* loginDialog = new Dialog_log();
    Communication::getInstance();
    loginDialog->show();

    if (loginDialog->exec() == QDialog::Accepted)
    {
        int clientId = Communication::getInstance()->getUserInfo()["ClientId"].toInteger();
        if (clientId >= 1000) {
            MainWindow_Admin* adminWin = new MainWindow_Admin();
            adminWin->setAttribute(Qt::WA_DeleteOnClose);
            adminWin->show();
        } else {
            MainWindow* clientWin = new MainWindow();
            clientWin->setAttribute(Qt::WA_DeleteOnClose);
            clientWin->show();
        }
        return app.exec();
    }
    return 0;
}
