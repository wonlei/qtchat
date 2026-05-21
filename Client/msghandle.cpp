#include "index.h"
#include "msghandle.h"

#include <QMessageBox>
#include <qmessagebox.h>

MsgHandle::MsgHandle(Client* client) : m_pClient(client)
{

}

static QString errorText(uint16_t code)
{
    switch (static_cast<ErrorCode>(code)) {
    case ERR_NONE:            return "";
    case ERR_AUTH_FAILED:     return "用户名或密码错误";
    case ERR_USER_EXISTS:     return "用户已存在";
    case ERR_USER_NOT_FOUND:  return "用户不存在";
    case ERR_USER_OFFLINE:    return "用户不在线";
    case ERR_ALREADY_FRIEND:  return "已经是好友";
    case ERR_PATH_UNSAFE:     return "路径不安全";
    case ERR_DIR_EXISTS:      return "目录已存在";
    case ERR_DIR_CREATE_FAILED: return "创建目录失败";
    case ERR_FILE_OPEN_FAILED:  return "文件打开失败";
    case ERR_FILE_UPLOAD_FAILED: return "文件上传失败";
    case ERR_SHARE_FILE_FAILED:  return "分享文件失败";
    case ERR_DELETE_FAILED:   return "删除失败";
    case ERR_RENAME_FAILED:   return "重命名失败";
    default:                  return QString("未知错误(%1)").arg(code);
    }
}

void MsgHandle::regist()
{
    if(!pdu || !m_pClient) {
            qDebug() << "Error: pdu or m_pClient is null";
            return;
      }
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    QMessageBox::information(m_pClient, "提示", "注册成功");
}

void MsgHandle::login()
{
    if(!pdu || !m_pClient) {
            qDebug() << "Error: pdu or m_pClient is null";
            return;
        }

    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    m_pClient->setLoggedIn();

    // Process offline messages delivered with login response
    int offlineCount = 0;
    memcpy(&offlineCount, pdu->caData + 4, sizeof(int));
    if (offlineCount > 0) {
        QStringList senders;
        int offset = 0;
        for (int i = 0; i < offlineCount; i++) {
            char senderBuf[32] = {'\0'};
            memcpy(senderBuf, pdu->caMsg + offset, 32);
            offset += 32;
            uint contentLen = 0;
            memcpy(&contentLen, pdu->caMsg + offset, sizeof(uint));
            offset += sizeof(uint);
            offset += contentLen;
            senders.append(QString::fromUtf8(senderBuf));
        }
        QMessageBox::information(m_pClient, "离线消息",
            QString("您有 %1 条离线消息，来自: %2").arg(offlineCount).arg(senders.join(", ")));
    }

    Index::getInstance().show();
    m_pClient->hide();
}

void MsgHandle::findUser()
{
    if(!pdu || !m_pClient) {
            qDebug() << "Error: pdu or m_pClient is null";
            return;
      }
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    int ret;
    memcpy(&ret, pdu->caData + 2, sizeof(int));
    if (code != ERR_NONE) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    if(ret == 0){
        QMessageBox::information(m_pClient, "提示", "用户没在线");
    }else if(ret == 1){
        QMessageBox::information(m_pClient, "提示", "用户在线");
    }else {
        QMessageBox::information(m_pClient, "提示", "用户不存在");
    }
}

void MsgHandle::onlineUser()
{
    if(!pdu || !m_pClient) {
            return;
      }
    int isize = pdu->uiMsgLen/32;
    QStringList nameList;
    for(int i=0;i<isize;i++){
        char caTmp[32] = {'\0'};
        memcpy(caTmp,pdu->caMsg+i*32,32);
        nameList.append(caTmp);
    }
    Index::getInstance().getfriend()->m_pOnlineUser->updateOnlineUser(nameList);
}

void MsgHandle::addFriend()
{
    if(!pdu || !m_pClient) {
            qDebug() << "Error: pdu or m_pClient is null";
            return;
      }
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    int ret;
    memcpy(&ret, pdu->caData + 2, sizeof(int));
    if (code != ERR_NONE) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    if(ret == -2){
        QMessageBox::information(m_pClient, "提示", "已经成为好友");
    }else{
        QMessageBox::information(m_pClient, "提示", "用户不存在");
    }
}

void MsgHandle::addFriendResend()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    int ret = QMessageBox::question(m_pClient,"添加好友",QString("用户 %1 添加您为好友，是否同意").arg(caName));
    if(ret == QMessageBox::Yes){
        PDUPtr respdu = makePDU();
        respdu->uiType =ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT_REQUEST;
        memcpy(respdu->caData,pdu->caData,64);
        m_pClient->sendMsg(respdu.get());
    }else{
        return;
    }
}


void MsgHandle::addFriendAgree()
{
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    Index::getInstance().getfriend()->flushFriend();
}

void MsgHandle::flushFriend()
{
    int isize = pdu->uiMsgLen/32;
    QStringList caName;

    char caTmp[32] = {'\0'};
    for(int i=0;i<isize;i++){
        memcpy(caTmp,pdu->caMsg+i*32,32);
        caName.append(caTmp);
    }

    Index::getInstance().getfriend()->updataFriend(caName);
}

void MsgHandle::deleteFriend()
{
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    Index::getInstance().getfriend()->flushFriend();
}

void MsgHandle::chat()
{
    char caChatName[32] = {'\0'};
    memcpy(caChatName,pdu->caData,32);
    Chat* c = Index::getInstance().getfriend()->m_pChat;
    c->updateShow_TE(QString("%1: %2").arg(caChatName).arg(pdu->caMsg));
    if(c->isHidden()){
        c->show();
    }
    return;

}

void MsgHandle::chatHistory()
{
    Chat* c = Index::getInstance().getfriend()->m_pChat;
    c->loadHistory(pdu);
}

void MsgHandle::createFlieShow()
{
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    Index::getInstance().getfile()->flushfile();
}

void MsgHandle::flushFile()
{
    if(!pdu) {
        qDebug() << "Error: pdu is null";
        return;
    }

    int icount = pdu->uiMsgLen/sizeof(FILE_INFO);
    QList<FILE_INFO*>pFileList;
    for(int i=0;i<icount;i++){
        FILE_INFO* pFileInfo = new FILE_INFO;
        //memcpy(pFileInfo,pdu->caMsg+i*sizeof(FILE_INFO),sizeof(FILE_INFO));
        memcpy(pFileInfo,(FILE_INFO*)pdu->caMsg+i,sizeof(FILE_INFO));
        pFileList.append(pFileInfo);
    }
    Index::getInstance().getfile()->updateFileList(pFileList);
}

void MsgHandle::deleteFile()
{
    if(!pdu){
        return;
    }
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    Index::getInstance().getfile()->flushfile();
}

void MsgHandle::renameFile()
{
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    Index::getInstance().getfile()->flushfile();
}

void MsgHandle::uploadFile()
{
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    Index::getInstance().getfile()->uploadFile();
}

void MsgHandle::uploadFileDeal()
{
    Index::getInstance().getfile()->flushfile();
}

void MsgHandle::shareFile()
{
    QMessageBox::information(m_pClient, "提示", "文件已分享");
}

void MsgHandle::shareFileDeal()
{
    QString shareFilePath = pdu->caMsg;
    qDebug() << "===== 开始文件复制 =====";
    qDebug() << "1. 源文件路径:" << shareFilePath;
    qDebug() << "   是否为空:" << shareFilePath.isEmpty();
    qDebug() << "   是否为null:" << shareFilePath.isNull();
    char senderName[32] = {'\0'};
    memcpy(senderName,pdu->caData,32);
    int index = shareFilePath.lastIndexOf('/');
    QString shareFileName = shareFilePath.right(shareFilePath.size()-index-1);
    int ret = QMessageBox::question(m_pClient,"分享文件",QString("用户 %1 给您分享文件 %2，是否同意").arg(senderName).arg(shareFileName));
    if(ret == QMessageBox::Yes){
        PDUPtr respdu = makePDU(pdu->uiMsgLen);
        respdu->uiType =ENUM_MSG_TYPE_SHARE_FILE_ACCEPT_REQUEST;
        memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
        memcpy(respdu->caData,m_pClient->m_strLoginName.toStdString().c_str(),32);
        m_pClient->sendMsg(respdu.get());
    }else{
        return;
    }
}

void MsgHandle::shareFileAgree()
{
    uint16_t code;
    memcpy(&code, pdu->caData, sizeof(uint16_t));
    bool ret;
    memcpy(&ret, pdu->caData + 2, sizeof(bool));
    if (code != ERR_NONE || !ret) {
        QMessageBox::information(m_pClient, "提示", errorText(code));
        return;
    }
    QMessageBox::information(m_pClient, "提示", "分享文件成功");
}
