#include "client.h"
#include "index.h"
#include "../common/protocol.h"
#include "ui_client.h"
#include "msghandle.h"
#include <QDebug>


#include <QFile>
#include <QHostAddress>
#include <QMessageBox>
#include <QCryptographicHash>
#include <qmessagebox.h>

Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    loadConfig();
    socket.connectToHost(QHostAddress(m_strlIP),m_usPort);
    connect(&socket,&QTcpSocket::connected,this,&Client::showConnect);
    connect(&socket,&QTcpSocket::readyRead,this,&Client::recvMsg);
}



void Client::loadConfig()
{
    QFile file(":/connect.config");
    if(file.open(QIODevice::ReadOnly)){
        QString s = file.readAll();
        QStringList slist=s.split("\r\n");
        m_strlIP=slist[0];
        m_usPort=slist[1].toUShort();
        m_strRootPath =slist[2];
        qDebug()<<"m_strlIP"<<m_strlIP<<" m_usPort"<<m_usPort;
        file.close();
    }else{
        qDebug() << "打印失败";
    }
}

Client::~Client()
{
    delete ui;
}

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

void Client::sendMsg(const PDU *pdu)
{
    if(pdu == nullptr) return;
    socket.write((char*)pdu, pdu->uiTotalLen);
    qDebug()<<"pdu->uiMsgLen "<<pdu->uiMsgLen
           <<"pdu->uiTotalLen "<<pdu->uiTotalLen
          <<"pdu->uiType "<<pdu->uiType
         <<"pdu->caData "<<pdu->caData
           <<"pdu->caData+32 "<<pdu->caData+32
        <<"pdu->caMsg "<<pdu->caMsg;
}


void Client::handleMsg(PDU *pdu)
{
    if(pdu == NULL)return;
    MsgHandle m_pmh(this);
    m_pmh.pdu = pdu;
    switch (pdu->uiType) {
    case ENUM_MSG_TYPE_REGISE_RESPOND: {
        m_pmh.regist();
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND: {
        m_pmh.login();
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_RESPOND: {
        m_pmh.findUser();
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_RESPOND:{
        m_pmh.onlineUser();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        m_pmh.addFriend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        m_pmh.addFriendResend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND:{
        m_pmh.addFriendAgree();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
        m_pmh.flushFriend();
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:{
        m_pmh.deleteFriend();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        m_pmh.chat();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_HISTORY_RESPOND:{
        m_pmh.chatHistory();
        break;
    }
    case ENUM_MSG_TYPE_CREATE_FILE_RESPOND:{
        m_pmh.createFlieShow();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:{
        m_pmh.flushFile();
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_RESPOND:{
        m_pmh.deleteFile();
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:{
        m_pmh.renameFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_INIT_RESPOND:{
        m_pmh.uploadFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DEAL_RESPOND:{
        m_pmh.uploadFileDeal();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
        m_pmh.shareFile();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
        m_pmh.shareFileDeal();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND:{
        m_pmh.shareFileAgree();
        break;
    }
    default:
        break;
    }
    return;
}


void Client::showConnect()
{
    qDebug()<<"连接服务器成功";
}





void Client::on_regist_PB_clicked()
{
    if (socket.state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "错误", "未连接到服务器，请稍后重试");
        return;
    }
    QString namestr = ui->name_LE->text();
    QString pwdstr = ui->pwd_LE->text();
    if(namestr.isEmpty() || pwdstr.isEmpty()
            || namestr.size()>32 || pwdstr.size() >32){
        QMessageBox::information(this,"错误","用户名或者密码非法");
        return;
    }
    PDUPtr pdu = makePDU();
    pdu->uiType = ENUM_MSG_TYPE_REGISE_REQUEST;
    memcpy(pdu->caData,namestr.toStdString().c_str(),32);
    QByteArray pwdHash = QCryptographicHash::hash(pwdstr.toUtf8(), QCryptographicHash::Sha256);
    memcpy(pdu->caData+32, pwdHash.constData(), 32);
    sendMsg(pdu.get());
}


void Client::recvMsg()
{
    if (m_bRecving) return;
    m_bRecving = true;

    QByteArray data = socket.readAll();
    buffer.append(data);
    while(buffer.size()>=int(sizeof(PDU))){
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size()<int(pdu->uiTotalLen)){
            break;
        }
        handleMsg(pdu);
        buffer.remove(0,pdu->uiTotalLen);
    }

    m_bRecving = false;
}

void Client::on_login_PB_clicked()
{
    if (socket.state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "错误", "未连接到服务器，请稍后重试");
        return;
    }
    QString namestr = ui->name_LE->text();
    QString pwdstr = ui->pwd_LE->text();
    if(namestr.isEmpty() || pwdstr.isEmpty()
            || namestr.size()>32 || pwdstr.size() >32){
        QMessageBox::information(this,"错误","用户名或者密码非法");
        return;
    }
    PDUPtr pdu = makePDU();
    pdu->uiType = ENUM_MSG_TYPE_LOGIN_REQUEST;
    m_strLoginName = namestr;
    memcpy(pdu->caData,namestr.toStdString().c_str(),32);
    QByteArray pwdHash = QCryptographicHash::hash(pwdstr.toUtf8(), QCryptographicHash::Sha256);
    memcpy(pdu->caData+32, pwdHash.constData(), 32);
    sendMsg(pdu.get());

}
