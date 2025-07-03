#include "mainwindow.h"
#include "mainwindow_admin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow clientWindow;
    clientWindow.setWindowTitle("💼 클라이언트 프로그램");
    clientWindow.show();

    MainWindow_Admin adminWindow;
    adminWindow.setWindowTitle("🔧 관리자 제품 관리");
    adminWindow.show();

    return app.exec();
}
