#ifndef MSGHANDLE_H
#define MSGHANDLE_H
#include "client.h"
#include "../common/protocol.h"

#include <QObject>

class MsgHandle
{
public:
    PDU* pdu;
    Client* m_pClient;  // 添加Client指针

    MsgHandle(Client* client = nullptr);
    void regist();
    void login();
    void findUser();
    void onlineUser();
    void addFriend();
    void addFriendResend();
    void addFriendAgree();
    void flushFriend();
    void deleteFriend();
    void chat();
    void chatHistory();
    void createFlieShow();
    void flushFile();
    void deleteFile();
    void renameFile();
    void uploadFile();
    void uploadFileDeal();
    void shareFile();
    void shareFileDeal();
    void shareFileAgree();
};
#endif // MSGHANDLE_H
