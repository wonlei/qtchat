#include "msghandler.h"
#include "mytcpserver.h"
#include "operatedb.h"
#include "server.h"
#include "../common/pathsafety.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <qmessagebox.h>
MsgHandler::MsgHandler(OperateDB& db, MyTcpServer* server, const QString& rootPath)
    : m_db(db), m_server(server), m_rootPath(rootPath)
{

}

PDUPtr MsgHandler::pathErrorResponse(ENUM_MSG_TYPE type, ErrorCode err)
{
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    respdu->uiType = type;
    uint16_t code = static_cast<uint16_t>(err);
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    return respdu;
}

PDUPtr MsgHandler::regist()
{
    char caname[32] = {'\0'};
    char capwd[32] = {'\0'};
    memcpy(caname,pdu->caData,32);
    memcpy(capwd,pdu->caData+32,32);
    bool ret  = m_db.handleRegist(caname,capwd);
    if(ret){
        QDir dir;
        dir.mkdir(QString("%1/%2").arg(m_rootPath).arg(caname));
    }
    qDebug() << "数据库操作结果:" << ret;
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_REGISTER_RESPOND;
    uint16_t code = ret ? ERR_NONE : ERR_USER_EXISTS;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    return respdu;
}

PDUPtr MsgHandler::login(QString &strName)
{
    char caname[32] = {'\0'};
    char capwd[32] = {'\0'};
    memcpy(caname,pdu->caData,32);
    memcpy(capwd,pdu->caData+32,32);
    bool ret  = m_db.handleLogin(caname,capwd);
    qDebug() << "数据库操作结果:" << ret;
    if(ret){
        strName = caname;
    }

    // Get offline messages on successful login
    QList<QPair<QString,QString>> offlineMsgs;
    if (ret) {
        offlineMsgs = m_db.getOfflineMessages(caname);
    }

    // Calculate payload size: [sender:32][contentLen:4][content] * N
    int msgSize = 0;
    for (const auto& m : offlineMsgs) {
        msgSize += 32 + sizeof(uint) + m.second.toUtf8().size();
    }

    PDUPtr respdu = makePDU(msgSize);
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    uint16_t code = ret ? ERR_NONE : ERR_AUTH_FAILED;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    int offlineCount = offlineMsgs.size();
    memcpy(respdu->caData + 4, &offlineCount, sizeof(int));

    int offset = 0;
    for (const auto& m : offlineMsgs) {
        char senderBuf[32] = {'\0'};
        strncpy(senderBuf, m.first.toUtf8().constData(), 31);
        memcpy(respdu->caMsg + offset, senderBuf, 32);
        offset += 32;
        QByteArray content = m.second.toUtf8();
        uint contentLen = content.size();
        memcpy(respdu->caMsg + offset, &contentLen, sizeof(uint));
        offset += sizeof(uint);
        memcpy(respdu->caMsg + offset, content.constData(), contentLen);
        offset += contentLen;
    }

    // Clear offline messages after delivery
    if (ret && offlineCount > 0) {
        m_db.clearOfflineMessages(caname);
        qDebug() << "login: delivered" << offlineCount << "offline messages to" << caname;
    }

    return respdu;
}

PDUPtr MsgHandler::findUser()
{
    char caname[32] = {'\0'};
    memcpy(caname,pdu->caData,32);
    int ret  = m_db.handleFinduser(caname);
    qDebug() << "数据库操作结果:" << ret;
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_FIND_USER_RESPOND;
    uint16_t code = (ret >= 0) ? ERR_NONE : ERR_USER_NOT_FOUND;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(int));
    return respdu;
}

PDUPtr MsgHandler::onlineUser()
{
    QStringList res = m_db.handleOnline();
    PDUPtr respdu = makePDU(res.size()*32);
    for(int i=0;i<res.size();i++){
        memcpy(respdu->caMsg+i*32,res[i].toStdString().c_str(),32);
    }
    respdu->uiType = ENUM_MSG_TYPE_ONLINE_USER_RESPOND;
    return respdu;
}

PDUPtr MsgHandler::addFrind()
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName,pdu->caData,32);
    memcpy(caTarName,pdu->caData+32,32);
    int ret  = m_db.handleAddFriend(caCurName,caTarName);
    qDebug() << "数据库操作结果:" << ret;
    if(ret == 1){
        m_server->resend(caTarName,pdu);
        return nullptr;
    }
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
    uint16_t code = (ret >= 0) ? ERR_NONE : ERR_ALREADY_FRIEND;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(int));
    return respdu;
}

PDUPtr MsgHandler::addFriendAgree()
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName,pdu->caData,32);
    memcpy(caTarName,pdu->caData+32,32);
    bool ret = m_db.handleAddFriendAgree(caCurName,caTarName);
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT_RESPOND;
    uint16_t code = ret ? ERR_NONE : ERR_USER_NOT_FOUND;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    m_server->resend(caCurName,respdu.get());
    return respdu;
}

PDUPtr MsgHandler::flushFriend()
{
    char curName[32] = {'\0'};
    memcpy(curName,pdu->caData,32);
    QStringList res = m_db.handleFlush(curName);
    PDUPtr respdu = makePDU(res.size()*32);
    qDebug()<<"好友列表的数量"<<res.size();
    for(int i=0;i<res.size();i++){
        memcpy(respdu->caMsg+i*32,res[i].toStdString().c_str(),32);
    }
    respdu->uiType = ENUM_MSG_TYPE_REFRESH_FRIEND_RESPOND;
    return respdu;
}

PDUPtr MsgHandler::deleteFriend()
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName,pdu->caData,32);
    memcpy(caTarName,pdu->caData+32,32);
    bool ret = m_db.handleDeleteFriend(caCurName,caTarName);
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
    uint16_t code = ret ? ERR_NONE : ERR_DELETE_FAILED;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    return respdu;
}

PDUPtr MsgHandler::chat()
{
    char sender[32] = {'\0'};
    char receiver[32] = {'\0'};
    memcpy(sender, pdu->caData, 32);
    memcpy(receiver, pdu->caData + 32, 32);
    m_db.saveMessage(sender, receiver, pdu->caMsg);

    if (m_db.isUserOnline(receiver)) {
        m_server->resend(receiver, pdu);
    } else {
        m_db.saveOfflineMessage(sender, receiver, pdu->caMsg);
        qDebug() << "chat: receiver offline, saved offline message from" << sender << "to" << receiver;
    }
    return nullptr;
}

PDUPtr MsgHandler::chatHistory()
{
    char user1[32] = {'\0'};
    char user2[32] = {'\0'};
    memcpy(user1, pdu->caData, 32);
    memcpy(user2, pdu->caData + 32, 32);

    QByteArray history = m_db.getChatHistory(user1, user2);
    PDUPtr respdu = makePDU(history.size());
    respdu->uiType = ENUM_MSG_TYPE_CHAT_HISTORY_RESPOND;
    memcpy(respdu->caMsg, history.constData(), history.size());
    return respdu;
}

PDUPtr MsgHandler::createFile()
{
    bool ret = false;
    QString errorMsg;
    // 1. 读取长度信息
    int pathSize = 0;
    int nameSize = 0;
    memcpy(&pathSize, pdu->caData, 4);
    memcpy(&nameSize, pdu->caData + 4, 4);
    QByteArray curPathData(pdu->caMsg, pathSize);
    QByteArray fileNameData(pdu->caMsg + pathSize, nameSize);
    QString curPath = QString::fromUtf8(curPathData);
    QString folderName = QString::fromUtf8(fileNameData);
    QString rootPath = m_rootPath;
    QString fullPath = QDir(curPath).absoluteFilePath(folderName);
    qDebug() << "createFile: curPath" << curPath << "folderName" << folderName << "fullPath" << fullPath << "rootPath" << rootPath;

    if (!isPathSafe(fullPath, rootPath)) {
        qDebug() << "createFile: PATH UNSAFE, returning error";
        return pathErrorResponse(ENUM_MSG_TYPE_CREATE_FILE_RESPOND);
    }

    QDir dir;
    if (dir.exists(fullPath)) {
        qDebug() << "目录已存在：" << fullPath;
        errorMsg = "目录已存在";
    } else {
        ret = dir.mkpath(fullPath);
        if (ret) {
            qDebug() << "目录创建成功：" << fullPath;
        } else {
            qDebug() << "目录创建失败：" << fullPath;
            errorMsg = "创建目录失败";
        }
    }
    PDUPtr respdu = makePDU(errorMsg.toUtf8().size());
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_CREATE_FILE_RESPOND;
    uint16_t code;
    if (ret) code = ERR_NONE;
    else if (errorMsg == "目录已存在") code = ERR_DIR_EXISTS;
    else code = ERR_DIR_CREATE_FAILED;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    if (!errorMsg.isEmpty()) {
        memcpy(respdu->caMsg, errorMsg.toUtf8().constData(), errorMsg.toUtf8().size());
    }

    return respdu;
}

PDUPtr MsgHandler::flushFile()
{
    QString dirPath = QString::fromUtf8(pdu->caMsg);
    if (!isPathSafe(dirPath, m_rootPath)) {
        return pathErrorResponse(ENUM_MSG_TYPE_REFRESH_FILE_RESPOND);
    }
    QDir dir(dirPath);
    QFileInfoList fileinfolist = dir.entryInfoList();
    PDUPtr respdu = makePDU(sizeof(FILE_INFO)*(fileinfolist.size()-2));
    respdu->uiType = ENUM_MSG_TYPE_REFRESH_FILE_RESPOND;
    for(int i = 0,j = 0 ;i<fileinfolist.size();i++){
        if(fileinfolist[i].fileName() == "."||fileinfolist[i].fileName() == ".."){
            continue;
        }
        FILE_INFO* pfileinfo = (FILE_INFO*)respdu->caMsg+j++;
        memcpy(pfileinfo->name,fileinfolist[i].fileName().toStdString().c_str(),32);
        if(fileinfolist[i].isDir()){
            pfileinfo->uiType = 0;
        }else{
            pfileinfo->uiType = 1;
        }
        qDebug()<<"caName"<<pfileinfo->name<<"caUiType"<<pfileinfo->uiType;
    }
    return respdu;
}

PDUPtr MsgHandler::deleteFile()
{
    QString strPath = QString::fromUtf8(pdu->caMsg);
    if (!isPathSafe(strPath, m_rootPath)) {
        return pathErrorResponse(ENUM_MSG_TYPE_DELETE_FILE_RESPOND);
    }
    uint uiType = 0;
    memcpy(&uiType,pdu->caData,sizeof(uint));
    bool ret;
    if(uiType == 0){
        QDir dir(strPath);
        ret = dir.removeRecursively();
    }else{
        QFile file(strPath);
        ret = file.remove();
    }
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    uint16_t code = ret ? ERR_NONE : ERR_DELETE_FAILED;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    respdu->uiType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
    return respdu;
}

PDUPtr MsgHandler::renameFile()
{
    char oldName[32] = {'\0'};
    char newName[32] = {'\0'};
    memcpy(oldName,pdu->caData,32);
    memcpy(newName,pdu->caData+32,32);
    QString strPath = QString::fromUtf8(pdu->caMsg);
    QString strOldPath = QString("%1/%2").arg(strPath).arg(oldName);
    QString strNewPath = QString("%1/%2").arg(strPath).arg(newName);

    QString rootPath = m_rootPath;
    if (!isPathSafe(strOldPath, rootPath) || !isPathSafe(strNewPath, rootPath)) {
        return pathErrorResponse(ENUM_MSG_TYPE_RENAME_FILE_RESPOND);
    }

    QDir dir;
    bool ret = dir.rename(strOldPath,strNewPath);
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    uint16_t code = ret ? ERR_NONE : ERR_RENAME_FAILED;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    respdu->uiType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
    return respdu;
}

PDUPtr MsgHandler::uploadFile()
{
    char caFileName[32] = {'\0'};
    memcpy(caFileName,pdu->caData,32);
    memcpy(&m_iUploadSize,pdu->caData+32,sizeof(qint64));
    m_iReceivedSize = 0;
    
    char* strCurPath = pdu->caMsg;
    QString curPath = QString::fromUtf8(strCurPath);
    QString rootPath = m_rootPath;
    QString fileName = QString(caFileName);

    QString fullPath = QDir(curPath).absoluteFilePath(fileName);
    qDebug() << "uploadFile: curPath" << curPath << "fileName" << fileName << "fullPath" << fullPath << "rootPath" << rootPath;
    if (!isPathSafe(fullPath, rootPath)) {
        qDebug() << "uploadFile: PATH UNSAFE, returning error";
        return pathErrorResponse(ENUM_MSG_TYPE_UPLOAD_FILE_INIT_RESPOND);
    }

    if (m_fUploadFile.isOpen()) {
        m_fUploadFile.close();
    }
    
    m_fUploadFile.setFileName(fullPath);
    bool ret = m_fUploadFile.open(QIODevice::WriteOnly);
    if (ret) {
        qDebug() << "文件打开成功，开始上传：" << fullPath;
    } else {
        qDebug() << "文件打开失败：" << fullPath << "，错误：" << m_fUploadFile.errorString();
    }
    
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_INIT_RESPOND;
    uint16_t code = ret ? ERR_NONE : ERR_FILE_OPEN_FAILED;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    return respdu;
}

PDUPtr MsgHandler::uploadFileDeal()
{
    m_fUploadFile.write(pdu->caMsg, pdu->uiMsgLen);
    m_iReceivedSize += pdu->uiMsgLen;
    
    // 如果上传未完成，返回NULL
    if (m_iReceivedSize < m_iUploadSize) {
        return nullptr;
    }
    
    // 上传完成，关闭文件
    m_fUploadFile.close();
    
    
    PDUPtr respdu = makePDU();
    respdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_DEAL_RESPOND;
    return respdu;
}

PDUPtr MsgHandler::shareFile()
{

    int friendCount = 0;
    char userName[32] = {'\0'};
    memcpy(userName,pdu->caData,32);
    memcpy(&friendCount,pdu->caData+32,sizeof(int));
    PDUPtr respdu = makePDU(pdu->uiMsgLen-friendCount*32);
    respdu->uiType = pdu->uiType;
    memcpy(respdu->caData,userName,32);
    memcpy(respdu->caMsg,pdu->caMsg+friendCount*32,pdu->uiMsgLen-friendCount*32);
    QString shareFilePath = QString::fromUtf8(respdu->caMsg);

    if (!isPathSafe(shareFilePath, m_rootPath)) {
        return pathErrorResponse(ENUM_MSG_TYPE_SHARE_FILE_RESPOND);
    }

    char Tmp[32] = {'\0'};
    for(int i = 0;i<friendCount;i++){
        memcpy(Tmp,pdu->caMsg+i*32,32);
        m_server->resend(Tmp,respdu.get());

    }
    respdu = makePDU();
    respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
    return respdu;

}

PDUPtr MsgHandler::shareFileAgree()
{
    QString shareFilePath = QString(pdu->caMsg);
    qDebug() << "===== 开始文件复制 =====";
    qDebug() << "1. 源文件路径:" << shareFilePath;
    qDebug() << "   是否为空:" << shareFilePath.isEmpty();
    qDebug() << "   是否为null:" << shareFilePath.isNull();

    // 安全提取文件名
    int index = shareFilePath.lastIndexOf('/');
    QString shareFileName = shareFilePath.right(shareFilePath.size()-index-1);
    qDebug() << "2. 提取的文件名:" << shareFileName;
    qDebug() << "   是否为空:" << shareFileName.isEmpty();

    // 检查用户名
    QString userName = QString(pdu->caData);
    qDebug() << "3. 用户名:" << userName;
    qDebug() << "   是否为空:" << userName.isEmpty();

    // 检查根路径
    QString rootPath = m_rootPath;
    qDebug() << "4. 根路径:" << rootPath;
    qDebug() << "   是否为空:" << rootPath.isEmpty();

    QString strTarPath = QString("%1/%2/%3").arg(rootPath).arg(userName).arg(shareFileName);
    qDebug() << "1. 新建文件路径:" << strTarPath;

    if (!isPathSafe(shareFilePath, rootPath) || !isPathSafe(strTarPath, rootPath)) {
        return pathErrorResponse(ENUM_MSG_TYPE_SHARE_FILE_ACCEPT_RESPOND);
    }

    bool ret = QFile::copy(shareFilePath,strTarPath);
    qDebug()<<"ret"<<ret;
    PDUPtr respdu = makePDU();
    if (!respdu) return nullptr;
    respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_ACCEPT_RESPOND;
    uint16_t code = ret ? ERR_NONE : ERR_SHARE_FILE_FAILED;
    memcpy(respdu->caData, &code, sizeof(uint16_t));
    memcpy(respdu->caData + 2, &ret, sizeof(bool));
    return respdu;
}


