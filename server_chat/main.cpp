#include "serverchatpartwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerChatPartWindow w;
    w.show();
    return a.exec();
}
