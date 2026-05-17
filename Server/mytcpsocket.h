#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "msghandler.h"
#include "../common/protocol.h"

#include <memory>
#include <QObject>
#include <qtcpsocket.h>
#include <QDebug>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:

    QString m_strLoginName;
    std::unique_ptr<MsgHandler> m_pmh;
    QByteArray buffer;
    MyTcpSocket();
     ~MyTcpSocket();
public slots:
    PDUPtr handleMsg(PDU* pdu);
    void recvMsg();
    void clientOffLine();
    void sendMsg(const PDU* respdu);

};

#endif // MYTCPSOCKET_H
