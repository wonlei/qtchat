#include "chat.h"
#include "../common/protocol.h"
#include "ui_chat.h"
#include "client.h"
Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_send_PB_clicked()
{
    QString sendMsg = ui->input_LE->text();
    if (sendMsg.isEmpty()) {
        return;
    }
    PDUPtr pdu = makePDU(sendMsg.toStdString().size() + 1);
    ui->input_LE->clear();
    pdu->uiType = ENUM_MSG_TYPE_CHAT_REQUEST;
    std::string loginName = Client::getInstance().m_strLoginName.toStdString();
    std::string chatName = m_strChatName.toStdString();
    std::string msgStr = sendMsg.toStdString();
    memcpy(pdu->caData, loginName.c_str(), 32);
    memcpy(pdu->caData + 32, chatName.c_str(), 32);
    memcpy(pdu->caMsg, msgStr.c_str(), msgStr.size());
    Client::getInstance().sendMsg(pdu.get());
}

void Chat::updateShow_TE( QString Msg)
{
    ui->show_TE->append(Msg);
    return;
}

void Chat::requestHistory()
{
    PDUPtr pdu = makePDU();
    pdu->uiType = ENUM_MSG_TYPE_CHAT_HISTORY_REQUEST;
    std::string loginName = Client::getInstance().m_strLoginName.toStdString();
    std::string chatName = m_strChatName.toStdString();
    memcpy(pdu->caData, loginName.c_str(), 32);
    memcpy(pdu->caData + 32, chatName.c_str(), 32);
    Client::getInstance().sendMsg(pdu.get());
}

void Chat::loadHistory(PDU *pdu)
{
    ui->show_TE->clear();
    int count = 0;
    memcpy(&count, pdu->caMsg, sizeof(int));
    int offset = sizeof(int);
    for (int i = 0; i < count; i++) {
        char sender[32] = {'\0'};
        memcpy(sender, pdu->caMsg + offset, 32);
        offset += 32;
        uint contentLen = 0;
        memcpy(&contentLen, pdu->caMsg + offset, sizeof(uint));
        offset += sizeof(uint);
        QByteArray content(pdu->caMsg + offset, contentLen);
        offset += contentLen;
        ui->show_TE->append(QString("%1: %2").arg(sender).arg(QString::fromUtf8(content)));
    }
}
