#include "clienttask.h"
#include "mytcpserver.h"
#include "mytcpsocket.h"
#include <QList>

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug() << "连接客户端成功";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);

    {
        QMutexLocker locker(&m_mutex);
        m_tcpSocketList.append(pTcpSocket);
        for(int i = 0; i < m_tcpSocketList.size(); i++) {
            qDebug() << m_tcpSocketList[i];
        }
    }

    ClientTask * task = new ClientTask(pTcpSocket);
    threadPool->start(task);
}

void MyTcpServer::removeSocket(MyTcpSocket *mytcpsocket)
{
    QMutexLocker locker(&m_mutex);
    if (m_tcpSocketList.removeOne(mytcpsocket)) {
        mytcpsocket->deleteLater();
        mytcpsocket = NULL;
    }
    for(int i = 0; i<m_tcpSocketList.size();i++){
        qDebug()<<m_tcpSocketList[i]->m_strLoginName;
    }
}

void MyTcpServer::resend(const char *caTarName, const PDU *pdu)
{
    if(caTarName == nullptr || pdu == nullptr){
        return;
    }
    QMutexLocker locker(&m_mutex);
    for(int i=0;i<m_tcpSocketList.size();i++){
        if(caTarName == m_tcpSocketList[i]->m_strLoginName){
            m_tcpSocketList[i]->write((char*)pdu,pdu->uiTotalLen);
            qDebug()<<"pdu->uiMsgLen "<<pdu->uiMsgLen
                   <<"pdu->uiTotalLen "<<pdu->uiTotalLen
                  <<"pdu->uiType "<<pdu->uiType
                 <<"pdu->caData "<<pdu->caData
                <<"pdu->caMsg "<<pdu->caMsg;
        }
    }
}




MyTcpServer::MyTcpServer()
{
    threadPool = QThreadPool::globalInstance();
    threadPool->setMaxThreadCount(8);
}
