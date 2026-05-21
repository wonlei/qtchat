#include "mytcpsocket.h"
#include "mytcpserver.h"
#include "../common/protocol.h"
#include "../common/packet.h"
#include "operatedb.h"
#include <QtDebug>
#include <QSslKey>
#include <QSslCertificate>
#include <QFile>
#include <qsqlquery.h>




MyTcpSocket::MyTcpSocket(OperateDB& db, MyTcpServer* server, const QString& rootPath)
    : m_db(db), m_server(server)
{
    m_pmh.reset(new MsgHandler(db, server, rootPath));
    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setSingleShot(true);
    connect(m_heartbeatTimer, &QTimer::timeout, this, [this]() {
        qDebug() << "Heartbeat timeout, disconnecting:" << m_strLoginName;
        this->disconnectFromHost();
    });

    // Register all message handlers
    auto& pmh = m_pmh;
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_REGISTER_REQUEST, [&pmh](PDU* p) { return pmh->regist(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_LOGIN_REQUEST, [this](PDU* p) { return m_pmh->login(m_strLoginName); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_FIND_USER_REQUEST, [&pmh](PDU* p) { return pmh->findUser(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_ONLINE_USER_REQUEST, [&pmh](PDU* p) { return pmh->onlineUser(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, [&pmh](PDU* p) { return pmh->addFrind(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT_REQUEST, [&pmh](PDU* p) { return pmh->addFriendAgree(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_REFRESH_FRIEND_REQUEST, [&pmh](PDU* p) { return pmh->flushFriend(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST, [&pmh](PDU* p) { return pmh->deleteFriend(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_CHAT_REQUEST, [&pmh](PDU* p) { return pmh->chat(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_CHAT_HISTORY_REQUEST, [&pmh](PDU* p) { return pmh->chatHistory(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_CREATE_FILE_REQUEST, [&pmh](PDU* p) { return pmh->createFile(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_REFRESH_FILE_REQUEST, [&pmh](PDU* p) { return pmh->flushFile(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_DELETE_FILE_REQUEST, [&pmh](PDU* p) { return pmh->deleteFile(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_RENAME_FILE_REQUEST, [&pmh](PDU* p) { return pmh->renameFile(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_UPLOAD_FILE_INIT_REQUEST, [&pmh](PDU* p) { return pmh->uploadFile(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_UPLOAD_FILE_DEAL_REQUEST, [&pmh](PDU* p) { return pmh->uploadFileDeal(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_SHARE_FILE_REQUEST, [&pmh](PDU* p) { return pmh->shareFile(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_SHARE_FILE_ACCEPT_REQUEST, [&pmh](PDU* p) { return pmh->shareFileAgree(); });
    m_dispatcher.registerHandler(ENUM_MSG_TYPE_HEARTBEAT_REQUEST, [](PDU*) {
        PDUPtr resp = makePDU();
        resp->uiType = ENUM_MSG_TYPE_HEARTBEAT_RESPOND;
        return resp;
    });
}

MyTcpSocket::~MyTcpSocket()
{
}




PDUPtr MyTcpSocket::handleMsg(PDU *pdu)
{
    if (!pdu) return nullptr;
    m_pmh->pdu = pdu;
    return m_dispatcher.dispatch(pdu);
}

void MyTcpSocket::startEncryption()
{
    QFile certFile("certs/server.crt");
    QFile keyFile("certs/server.key");
    if (!certFile.open(QIODevice::ReadOnly) || !keyFile.open(QIODevice::ReadOnly)) {
        qWarning() << "TLS certificates not found, falling back to plain TCP";
        m_bUseTls = false;
        return;
    }
    QSslCertificate cert(&certFile, QSsl::Pem);
    QSslKey key(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();

    if (cert.isNull() || key.isNull()) {
        qWarning() << "Invalid TLS certificate or key";
        m_bUseTls = false;
        return;
    }

    setLocalCertificate(cert);
    setPrivateKey(key);
    startServerEncryption();
    qDebug() << "TLS server encryption started";
}

void MyTcpSocket::recvMsg()
{
    m_heartbeatTimer->start(HEARTBEAT_TIMEOUT_MS);
    QByteArray data = this->readAll();
    buffer.append(data);

    while (buffer.size() >= 4) {
        // Detect format: high bit of first uint32 = new Frame, otherwise old PDU
        uint32_t firstWord;
        memcpy(&firstWord, buffer.constData(), 4);
        bool isFrame = (firstWord & Frame::MAGIC_MASK) != 0;

        if (isFrame) {
            uint32_t totalLen = firstWord & ~Frame::MAGIC_MASK;
            if (buffer.size() < static_cast<int>(totalLen)) break;

            auto f = Frame::deserialize(buffer.left(totalLen));
            buffer.remove(0, totalLen);

            if (!f) continue;

            // Convert Frame to PDU for legacy handler
            uint metaLen = qMin(f->meta.size(), 64);
            PDUPtr pdu = makePDU(f->payload.size());
            pdu->uiType = f->msg_type;
            if (metaLen > 0)
                memcpy(pdu->caData, f->meta.constData(), metaLen);
            if (f->payload.size() > 0)
                memcpy(pdu->caMsg, f->payload.constData(), f->payload.size());

            PDUPtr respdu = handleMsg(pdu.get());
            sendMsg(respdu.get());
        } else {
            // Legacy PDU format
            if (buffer.size() < static_cast<int>(sizeof(PDU))) break;
            PDU* pdu = reinterpret_cast<PDU*>(buffer.data());
            if (buffer.size() < static_cast<int>(pdu->uiTotalLen)) break;

            PDUPtr respdu = handleMsg(pdu);
            sendMsg(respdu.get());
            buffer.remove(0, pdu->uiTotalLen);
        }
    }
}


void MyTcpSocket::clientOffLine()
{
    qDebug()<<"clientOffLine"<<m_strLoginName;
    m_db.clientOffLine(m_strLoginName.toStdString().c_str());
    m_server->removeSocket(this);
}

void MyTcpSocket::sendMsg(const PDU *respdu)
{
    if (respdu == nullptr) return;

    // Send in new Frame format
    QByteArray meta(respdu->caData, 64); // keep legacy 64-byte caData as meta
    QByteArray payload(respdu->caMsg, respdu->uiMsgLen);

    Frame f;
    f.msg_type = respdu->uiType;
    f.meta = meta;
    f.payload = payload;

    QByteArray frameData = f.serialize();
    this->write(frameData);
    qDebug() << "Frame sent:" << f.msg_type << "total_len:" << frameData.size();
}


