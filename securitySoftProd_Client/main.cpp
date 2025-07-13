#include "dialog_log.h"
#include "mainwindow_admin.h"
#include "mainwindow.h"

#include <QApplication>
#include <QJsonObject>
#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Dialog_log* loginDialog = Dialog_log::getInstance();

    if (loginDialog->exec() == QDialog::Accepted)
    {
        QTcpSocket* socket = loginDialog->getSocket();
        QJsonObject userInfo = loginDialog->getUserInfo();

        qint64 clientId = userInfo["ClientId"].toInteger();

        if (clientId >= 1000) {
            MainWindow_Admin* adminWin = new MainWindow_Admin(socket, userInfo, nullptr);
            adminWin->setAttribute(Qt::WA_DeleteOnClose);
            adminWin->show();
        } else {
            MainWindow* clientWin = new MainWindow(socket, userInfo, nullptr);
            clientWin->setAttribute(Qt::WA_DeleteOnClose);
            clientWin->show();
        }
        return app.exec();
    }
    return 0;
}
