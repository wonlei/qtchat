#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "msghandler.h"
#include "../common/messagedispatcher.h"
#include "../common/protocol.h"

#include <memory>
#include <QObject>
#include <QSslSocket>
#include <QTimer>
#include <QDebug>

class OperateDB;
class MyTcpServer;

class MyTcpSocket : public QSslSocket
{
    Q_OBJECT
public:

    QString m_strLoginName;
    std::unique_ptr<MsgHandler> m_pmh;
    MessageDispatcher m_dispatcher;
    QByteArray buffer;
    bool m_bUseTls = false;
    QTimer* m_heartbeatTimer;
    static const int HEARTBEAT_TIMEOUT_MS = 30000;
    MyTcpSocket(OperateDB& db, MyTcpServer* server, const QString& rootPath);
     ~MyTcpSocket();
    OperateDB& m_db;
    MyTcpServer* m_server;
    void startEncryption();
public slots:
    PDUPtr handleMsg(PDU* pdu);
    void recvMsg();
    void clientOffLine();
    void sendMsg(const PDU* respdu);

};

#endif // MYTCPSOCKET_H
