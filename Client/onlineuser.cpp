#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

Onlineuser::Onlineuser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Onlineuser)
{
    ui->setupUi(this);
}

Onlineuser::~Onlineuser()
{
    delete ui;
}

void Onlineuser::updateOnlineUser(QStringList nameList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(nameList);
}



void Onlineuser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strCurName = Client::getInstance().m_strLoginName;
    QString strTarName = item->text();
    PDUPtr pdu = makePDU();
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
    pdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    Client::getInstance().sendMsg(pdu.get());
}
