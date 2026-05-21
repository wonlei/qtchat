#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "../common/protocol.h"

#include <QObject>
#include <qfile.h>

class OperateDB;
class MyTcpServer;

class MsgHandler
{
public:
    PDU* pdu;
    QFile m_fUploadFile;
    qint64 m_iUploadSize;
    qint64 m_iReceivedSize;
    MsgHandler(OperateDB& db, MyTcpServer* server, const QString& rootPath);

    OperateDB& m_db;
    MyTcpServer* m_server;
    QString m_rootPath;

    static PDUPtr pathErrorResponse(ENUM_MSG_TYPE type, ErrorCode err = ERR_PATH_UNSAFE);

    PDUPtr regist();
    PDUPtr login(QString& strName);
    PDUPtr findUser();
    PDUPtr onlineUser();
    PDUPtr addFrind();
    PDUPtr addFriendAgree();
    PDUPtr flushFriend();
    PDUPtr deleteFriend();
    PDUPtr chat();
    PDUPtr chatHistory();
    PDUPtr createFile();
    PDUPtr flushFile();
    PDUPtr deleteFile();
    PDUPtr renameFile();
    PDUPtr uploadFile();
    PDUPtr uploadFileDeal();
    PDUPtr shareFile();
    PDUPtr shareFileAgree();
};

#endif // MSGHANDLER_H
