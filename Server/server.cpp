#include "server.h"
#include "mytcpserver.h"
#include "operatedb.h"
#include "../common/config.h"

#include <QDebug>

Server::Server(QWidget *parent )
    : QMainWindow(parent)
{
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strlIP),m_usPort);
}

Server::~Server()
{
    delete m_db;
}

void Server::loadConfig()
{
    AppConfig cfg;
    if (!cfg.loadFromFile(":/config.json")) {
        // Fallback to legacy format
        QFile file(":/connect.config");
        if (file.open(QIODevice::ReadOnly)) {
            cfg.loadLegacy(file.readAll());
            file.close();
        }
    }
    m_strlIP     = cfg.serverIp;
    m_usPort     = cfg.serverPort;
    m_strRootPath = cfg.rootPath;

    m_db = new OperateDB;
    m_db->setConfig(cfg);
    m_db->init();

    MyTcpServer::getInstance().setDb(*m_db);
    MyTcpServer::getInstance().setRootPath(m_strRootPath);
    MyTcpServer::getInstance().m_bUseTls = cfg.useTls;

    qDebug()<<"m_strlIP"<<m_strlIP<<" m_usPort"<<m_usPort<<" rootPath"<<m_strRootPath;
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

