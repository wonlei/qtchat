#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <qtcpserver.h>
#include <QDebug>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QThreadPool>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static MyTcpServer& getInstance();
    void setDb(OperateDB& db) { m_db = &db; }
    void setRootPath(const QString& path) { m_rootPath = path; }
    void incomingConnection(qintptr handle) override;
    void removeSocket(MyTcpSocket* mytcpsocket);
    void resend(const char* caTarName, const PDU* pdu);
    QThreadPool* threadPool;
    bool m_bUseTls = false;
private:
     MyTcpServer();
     OperateDB* m_db = nullptr;
     QString m_rootPath;
     MyTcpServer(const MyTcpServer& instance) = delete;
     MyTcpServer& operator=(const MyTcpServer&) = delete;
     mutable QMutex m_mutex;
     QList<MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H
