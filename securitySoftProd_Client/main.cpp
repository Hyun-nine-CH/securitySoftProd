#include "dialog_log.h"
#include "mainwindow_admin.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Dialog_log* login=Dialog_log::getInstance();
    if(login->exec()==QDialog::Accepted) {
    }

    return app.exec();

}
