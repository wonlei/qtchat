#include "client.h"
#include "index.h"
#include "../common/config.h"
#include "../common/protocol.h"
#include "../common/packet.h"
#include "ui_client.h"
#include "msghandle.h"
#include <QDebug>


#include <QFile>
#include <QHostAddress>
#include <QMessageBox>
#include <qmessagebox.h>

Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    loadConfig();
    if (m_bUseTls) {
        socket.connectToHostEncrypted(m_strlIP, m_usPort);
    } else {
        socket.connectToHost(QHostAddress(m_strlIP), m_usPort);
    }
    m_state = ConnectionState::Connecting;
    connect(&socket, &QSslSocket::connected, this, &Client::showConnect);
    connect(&socket, &QSslSocket::readyRead, this, &Client::recvMsg);
    connect(&socket, &QSslSocket::disconnected, this, [this]() {
        m_state = ConnectionState::Disconnected;
        qDebug() << "Disconnected from server";
    });

    m_heartbeatTimer = new QTimer(this);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &Client::sendHeartbeat);
    m_heartbeatTimer->start(15000);

    // Register message handlers
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_REGISTER_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.regist(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_LOGIN_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.login(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_FIND_USER_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.findUser(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_ONLINE_USER_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.onlineUser(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_ADD_FRIEND_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.addFriend(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.addFriendResend(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.addFriendAgree(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_REFRESH_FRIEND_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.flushFriend(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.deleteFriend(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_CHAT_REQUEST, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.chat(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_CHAT_HISTORY_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.chatHistory(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_CREATE_FILE_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.createFlieShow(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_REFRESH_FILE_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.flushFile(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_DELETE_FILE_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.deleteFile(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_RENAME_FILE_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.renameFile(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_UPLOAD_FILE_INIT_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.uploadFile(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_UPLOAD_FILE_DEAL_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.uploadFileDeal(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_SHARE_FILE_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.shareFile(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_SHARE_FILE_REQUEST, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.shareFileDeal(); return nullptr; });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_SHARE_FILE_ACCEPT_RESPOND, [this](PDU* p) {
        MsgHandle mh(this); mh.pdu = p; mh.shareFileAgree(); return nullptr; });
}



void Client::loadConfig()
{
    AppConfig cfg;
    if (!cfg.loadFromFile(":/config.json")) {
        // Fallback to legacy format
        QFile file(":/connect.config");
        if (file.open(QIODevice::ReadOnly)) {
            cfg.loadLegacy(file.readAll());
            file.close();
        }
    }
    m_strlIP     = cfg.serverIp;
    m_usPort     = cfg.serverPort;
    m_strRootPath = cfg.rootPath;
    m_bUseTls    = cfg.useTls;
    qDebug()<<"m_strlIP"<<m_strlIP<<" m_usPort"<<m_usPort<<" TLS"<<m_bUseTls;
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
    if (pdu == nullptr) return;

    // Send in new Frame format
    QByteArray meta(pdu->caData, 64);
    QByteArray payload(pdu->caMsg, pdu->uiMsgLen);

    Frame f;
    f.msg_type = pdu->uiType;
    f.meta = meta;
    f.payload = payload;

    QByteArray frameData = f.serialize();
    socket.write(frameData);
    qDebug() << "Frame sent:" << f.msg_type << "total_len:" << frameData.size();
}


void Client::sendHeartbeat()
{
    if (m_state != ConnectionState::LoggedIn) return;
    PDUPtr pdu = makePDU();
    if (!pdu) return;
    pdu->uiType = ENUM_MSG_TYPE_HEARTBEAT_REQUEST;
    sendMsg(pdu.get());
}

void Client::handleMsg(PDU *pdu)
{
    if (!pdu) return;
    m_dispatcher.dispatch(pdu);
}


void Client::showConnect()
{
    m_state = ConnectionState::Connected;
    qDebug()<<"连接服务器成功";
}





void Client::on_regist_PB_clicked()
{
    if (m_state < ConnectionState::Connected) {
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
    pdu->uiType = ENUM_MSG_TYPE_REGISTER_REQUEST;
    std::string name = namestr.toStdString();
    std::string pwd = pwdstr.toStdString();
    memcpy(pdu->caData, name.c_str(), 32);
    memcpy(pdu->caData + 32, pwd.c_str(), 32);
    sendMsg(pdu.get());
}


void Client::recvMsg()
{
    if (m_bRecving) return;
    m_bRecving = true;

    QByteArray data = socket.readAll();
    buffer.append(data);
    qDebug() << "Client recv: buffer size" << buffer.size();

    while (buffer.size() >= 4) {
        uint32_t firstWord;
        memcpy(&firstWord, buffer.constData(), 4);
        bool isFrame = (firstWord & Frame::MAGIC_MASK) != 0;

        if (isFrame) {
            uint32_t totalLen = firstWord & ~Frame::MAGIC_MASK;
            qDebug() << "Client: Frame detected, totalLen" << totalLen << "buffer" << buffer.size();
            if (buffer.size() < static_cast<int>(totalLen)) break;

            auto f = Frame::deserialize(buffer.left(totalLen));
            if (!f) {
                qDebug() << "Client: deserialize FAILED for totalLen" << totalLen;
                buffer.remove(0, totalLen);
                continue;
            }
            qDebug() << "Client: Frame OK type" << f->msg_type << "meta" << f->meta.size() << "payload" << f->payload.size();
            buffer.remove(0, totalLen);

            // Convert Frame to PDU for legacy handler
            uint metaLen = qMin(f->meta.size(), 64);
            PDUPtr pdu = makePDU(f->payload.size());
            pdu->uiType = f->msg_type;
            if (metaLen > 0)
                memcpy(pdu->caData, f->meta.constData(), metaLen);
            if (f->payload.size() > 0)
                memcpy(pdu->caMsg, f->payload.constData(), f->payload.size());

            handleMsg(pdu.get());
        } else {
            // Legacy PDU format
            if (buffer.size() < static_cast<int>(sizeof(PDU))) break;
            PDU* pdu = reinterpret_cast<PDU*>(buffer.data());
            if (buffer.size() < static_cast<int>(pdu->uiTotalLen)) break;

            handleMsg(pdu);
            buffer.remove(0, pdu->uiTotalLen);
        }
    }

    m_bRecving = false;
}

void Client::on_login_PB_clicked()
{
    if (m_state < ConnectionState::Connected) {
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
    std::string name = namestr.toStdString();
    std::string pwd = pwdstr.toStdString();
    memcpy(pdu->caData, name.c_str(), 32);
    memcpy(pdu->caData + 32, pwd.c_str(), 32);
    sendMsg(pdu.get());

}
