#include "index.h"
#include "msghandle.h"

#include <QMessageBox>
#include <qmessagebox.h>

MsgHandle::MsgHandle(Client* client) : m_pClient(client)
{

}

void MsgHandle::regist()
{
    if(!pdu || !m_pClient) {
            qDebug() << "Error: pdu or m_pClient is null";
            return;
      }
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret) {
        QMessageBox::information(m_pClient, "提示", "注册成功");
    } else {
        QMessageBox::information(m_pClient, "提示", "注册失败");
    }
}

void MsgHandle::login()
{
    if(!pdu || !m_pClient) {
            qDebug() << "Error: pdu or m_pClient is null";
            return;
        }

    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret) {
        Index::getInstance().show();
        m_pClient->hide();
    } else {
        QMessageBox::information(m_pClient, "提示", "登录失败");
    }
}

void MsgHandle::findUser()
{
    if(!pdu || !m_pClient) {
            qDebug() << "Error: pdu or m_pClient is null";
            return;
      }
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));
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
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));
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
        respdu->uiType =ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST;
        memcpy(respdu->caData,pdu->caData,64);
        m_pClient->sendMsg(respdu.get());
    }else{
        return;
    }
}


void MsgHandle::addFriendAgree()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret) {
        Index::getInstance().getfriend()->flushFriend();
    } else {
        QMessageBox::information(m_pClient, "提示", "添加好友失败");
    }
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
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getfriend()->flushFriend();
    }else{
        QMessageBox::information(m_pClient, "提示", "删除失败");
    }
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
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret) {
        Index::getInstance().getfile()->flushfile();
    } else {
        QMessageBox::information(m_pClient, "提示", "创建文件夹失败");
    }
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
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getfile()->flushfile();
    }else {
            QMessageBox::information(m_pClient, "提示", "删除文件夹失败");
    }
}

void MsgHandle::renameFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getfile()->flushfile();
    }else {
            QMessageBox::information(m_pClient, "提示", "重命名文件夹失败");
    }
}

void MsgHandle::uploadFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getfile()->uploadFile();
    }else {
            QMessageBox::information(m_pClient, "提示", "下载文件夹失败");
    }
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
        respdu->uiType =ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST;
        memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
        memcpy(respdu->caData,m_pClient->m_strLoginName.toStdString().c_str(),32);
        m_pClient->sendMsg(respdu.get());
    }else{
        return;
    }
}

void MsgHandle::shareFileAgree()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret) {
        QMessageBox::information(m_pClient, "提示", "分享文件成功");
    } else {
        QMessageBox::information(m_pClient, "提示", "分享文件失败");
    }
}

