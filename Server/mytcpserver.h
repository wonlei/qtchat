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
    void incomingConnection(qintptr handle) override;
    void removeSocket(MyTcpSocket* mytcpsocket);
    void resend(const char* caTarName, const PDU* pdu);
    QThreadPool* threadPool;
private:
     MyTcpServer();
     MyTcpServer(const MyTcpServer& instance) = delete;
     MyTcpServer& operator=(const MyTcpServer&) = delete;
     mutable QMutex m_mutex;
     QList<MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H
