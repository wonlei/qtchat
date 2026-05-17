#include "friend.h"
#include "../common/protocol.h"
#include "ui_friend.h"
#include "client.h"
#include "onlineuser.h"

#include <QInputDialog>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_pOnlineUser = new Onlineuser();
    m_pChat = new Chat();
    flushFriend();
}

Friend::~Friend()
{
    delete ui;
    delete m_pOnlineUser;
    delete m_pChat;
}

void Friend::flushFriend()
{
    PDUPtr pdu = makePDU();
    QString curName = Client::getInstance().m_strLoginName;
    memcpy(pdu->caData,curName.toStdString().c_str(),32);
    pdu->uiType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    Client::getInstance().sendMsg(pdu.get());
}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    m_pChat->m_strChatName = pItem->text();
    if(m_pChat->isHidden()){
        m_pChat->show();
    }
    m_pChat->requestHistory();
}

void Friend::on_findUser_PB_clicked()
{
    QString strName = QInputDialog::getText(this,"查找用户","用户名");
    if(strName.isEmpty()) {
           qDebug() << "用户取消查找或输入为空";
            return;  // 直接返回，不执行后续代码
    }
    PDUPtr pdu =makePDU();
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    pdu->uiType = ENUM_MSG_TYPE_FIND_USER_REQUEST;
    Client::getInstance().sendMsg(pdu.get());

}

void Friend::on_onlineUser_PB_clicked()
{
    qDebug() << "Button clicked, m_pOnlineUser:" << m_pOnlineUser;

    if(m_pOnlineUser == nullptr) {
        qDebug() << "Error: m_pOnlineUser is null!";
        return;
    }

    qDebug() << "Window is hidden:" << m_pOnlineUser->isHidden();

    if(m_pOnlineUser->isHidden()){
        m_pOnlineUser->show();
        qDebug() << "Onlineuser window shown";
    } else {
        m_pOnlineUser->activateWindow();  // 如果已经显示，激活窗口
        qDebug() << "Onlineuser window already visible, activated";
    }

    PDUPtr pdu = makePDU();
    pdu->uiType = ENUM_MSG_TYPE_ONLINE_USER_REQUEST;
    Client::getInstance().sendMsg(pdu.get());
    qDebug() << "Online user request sent";
}

void Friend::on_flush_PB_clicked()
{
    flushFriend();
}

void Friend::updataFriend(QStringList caName)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(caName);
}

QListWidget *Friend::getFriend_Lw()
{
    return ui->listWidget;
}

void Friend::on_del_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    QString caName = pItem->text();
    PDUPtr pdu = makePDU();
    memcpy(pdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,caName.toStdString().c_str(),32);
    pdu->uiType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    Client::getInstance().sendMsg(pdu.get());
}
