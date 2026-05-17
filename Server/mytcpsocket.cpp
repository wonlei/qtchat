#include "mytcpsocket.h"
#include "mytcpserver.h"
#include "../common/protocol.h"
#include "operatedb.h"
#include <QtDebug>
#include <qsqlquery.h>




MyTcpSocket::MyTcpSocket()
{
    m_pmh.reset(new MsgHandler);
}

MyTcpSocket::~MyTcpSocket()
{
}




PDUPtr MyTcpSocket::handleMsg(PDU *pdu)
{
    PDUPtr respdu;
    m_pmh->pdu = pdu;
    if(pdu == NULL) return nullptr;
    switch (pdu->uiType) {
    case ENUM_MSG_TYPE_REGISE_REQUEST:{
        respdu = m_pmh->regist();
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:{
        respdu = m_pmh->login(m_strLoginName);
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_REQUEST:{
        respdu = m_pmh->findUser();
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_REQUEST:{
        respdu = m_pmh->onlineUser();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        respdu = m_pmh->addFrind();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST:{
        respdu = m_pmh->addFriendAgree();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{
        respdu = m_pmh->flushFriend();
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:{
        respdu = m_pmh->deleteFriend();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        respdu = m_pmh->chat();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_HISTORY_REQUEST:{
        respdu = m_pmh->chatHistory();
        break;
    }
    case ENUM_MSG_TYPE_CREATE_FILE_REQUEST:{
        respdu = m_pmh->createFile();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:{
        respdu = m_pmh->flushFile();
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_REQUEST:{
        respdu = m_pmh->deleteFile();
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:{
        respdu = m_pmh->renameFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_INIT_REQUEST:{
        respdu = m_pmh->uploadFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DEAL_REQUEST:{
        respdu = m_pmh->uploadFileDeal();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
        respdu = m_pmh->shareFile();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST:{
        respdu = m_pmh->shareFileAgree();
        break;
    }
    default:
        break;
    }
    return respdu;
}

void MyTcpSocket::recvMsg()
{
    QByteArray data = this->readAll();
    buffer.append(data);
    while(buffer.size()>int(sizeof(PDU))){
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size()<int(pdu->uiTotalLen)){
            break;
        }
        PDUPtr respdu = handleMsg(pdu);
        sendMsg(respdu.get());
        buffer.remove(0,pdu->uiTotalLen);
    }
}


void MyTcpSocket::clientOffLine()
{
    qDebug()<<"clientOffLine"<<m_strLoginName;
    OperateDB::getInstance().clientOffLine(m_strLoginName.toStdString().c_str());
    MyTcpServer::getInstance().removeSocket(this);
}

void MyTcpSocket::sendMsg(const PDU *respdu)
{
    if(respdu == nullptr) return;
    this->write((char*)respdu, respdu->uiTotalLen);
    qDebug()<<"respdu->uiMsgLen "<<respdu->uiMsgLen
           <<"respdu->uiTotalLen "<<respdu->uiTotalLen
          <<"respdu->uiType "<<respdu->uiType
         <<"respdu->caData "<<respdu->caData
        <<"respdu->caMsg "<<respdu->caMsg;
}


