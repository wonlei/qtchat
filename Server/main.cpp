#include "operatedb.h"
#include "server.h"

#include <QApplication>
#include <QDebug>

//typedef struct ABC{
//    int a;
//    int b;
//    int c[];
//}ABC;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OperateDB::getInstance().connect();
    Server::getInstance().show();
    return a.exec();
}
