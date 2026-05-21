#include "logmanager.h"
#include "operatedb.h"
#include "server.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LogManager::init();
    Server::getInstance().show();
    int ret = a.exec();
    LogManager::cleanup();
    return ret;
}
