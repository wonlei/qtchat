#include "server.h"
#include "mytcpserver.h"

#include <QFile>
#include<QDebug>

Server::Server(QWidget *parent )
    : QMainWindow(parent)
{
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strlIP),m_usPort);
}

Server::~Server()
{
}

void Server::loadConfig()
{
    QFile file(":/connect.config");
    if(file.open(QIODevice::ReadOnly)){
        QString s = file.readAll();
        QStringList slist=s.split("\r\n");
        m_strlIP=slist[0];
        m_usPort=slist[1].toUShort();
        m_strRootPath = slist[2];
        //qDebug()<<m_strlIP<<" "<<m_usPort;
        file.close();
    }else{
        qDebug() << "打印失败";
    }
    qDebug()<<"m_strlIP"<<m_strlIP<<" m_usPort"<<m_usPort;
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

