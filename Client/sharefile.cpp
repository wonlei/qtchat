#include "client.h"
#include "index.h"
#include "sharefile.h"
#include "ui_sharefile.h"

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateList()
{
    ui->listWidget->clear();
    QListWidget* friend_Lw = Index::getInstance().getfriend()->getFriend_Lw();
    for (int i = 0;i<friend_Lw->count();i++) {
        QListWidgetItem* friendItem = friend_Lw->item(i);
        QListWidgetItem* newItem = new QListWidgetItem(*friendItem);
        ui->listWidget->addItem(newItem);
    }
}

void ShareFile::on_allSelect_pb_clicked()
{
    for (int i = 0;i<ui->listWidget->count();i++) {
        ui->listWidget->item(i)->setSelected(true);
    }
}

void ShareFile::on_cancelSelect_pb_clicked()
{
    for (int i = 0;i<ui->listWidget->count();i++) {
        ui->listWidget->item(i)->setSelected(false);
    }
}

void ShareFile::on_ok_pb_clicked()
{
    QString strName = Client::getInstance().m_strLoginName;
    QString strCurPath = Index::getInstance().getfile()->m_strCurPath;
    QString sharePath = QString("%1/%2").arg(strCurPath).arg(shareFileName);
    qDebug() << "分享文件名:" << shareFileName;
    qDebug() << "当前路径:" << strCurPath;
    qDebug() << "1. 源文件路径:" << sharePath;
    QList<QListWidgetItem*>pItems = ui->listWidget->selectedItems();
    int friendcount = pItems.size();
    PDUPtr pdu = makePDU(32*friendcount+sharePath.toStdString().size()+1);
    for(int i =0 ;i<friendcount;i++){
        memcpy(pdu->caMsg+i*32,ui->listWidget->item(i)->text().toStdString().c_str(),32);
    }
    memcpy(pdu->caMsg+friendcount*32,sharePath.toStdString().c_str(),sharePath.toStdString().size());
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,&friendcount,sizeof(int));
    pdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    Client::getInstance().sendMsg(pdu.get());

}
