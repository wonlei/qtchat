#include "client.h"
#include "file.h"

#include "ui_file.h"
#include "uploader.h"



#include <qinputdialog.h>
#include <QTimer>
#include <qmessagebox.h>
#include <QFileDialog>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strIniPath = QString("%1/%2").arg(Client::getInstance().m_strRootPath).arg(Client::getInstance().m_strLoginName);
    m_strCurPath = m_strIniPath;
    sharefile = new ShareFile;
    flushfile();
}

File::~File()
{

    delete ui;
    delete sharefile;
}

void File::uploadFile()
{
    Uploader* uploader = new Uploader(m_strUploadPath);
    connect(uploader,&Uploader::errorSignal,this,&File::errorBox,Qt::QueuedConnection);
    connect(uploader,&Uploader::uploadPDU,this,[&](PDU* rawPdu){
        PDUPtr pdu(rawPdu);
        Client::getInstance().sendMsg(pdu.get());
    },Qt::QueuedConnection);
    uploader->start();
}

void File::updateFileList(QList<FILE_INFO *> pFileList)
{
    foreach(FILE_INFO* pFileInfo,m_pFileInfoList){
        delete pFileInfo;
    }
    m_pFileInfoList = pFileList;
    ui->listWidget->clear();
    foreach(FILE_INFO* pFileINFO , pFileList){
        QListWidgetItem *pItem = new QListWidgetItem;
        if(pFileINFO->uiType == 0){
            pItem->setIcon(QPixmap(":/file.png"));
        }else{
            pItem->setIcon(QPixmap(":/text.png"));
        }
        pItem->setText(pFileINFO->name);
        qDebug()<<"filename"<<pFileINFO->name;
        ui->listWidget->addItem(pItem);
    }
}

void File::flushfile()
{
    PDUPtr pdu = makePDU(m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    Client::getInstance().sendMsg(pdu.get());
}


void File::on_upload_PB_clicked()
{
    m_strUploadPath = QFileDialog::getOpenFileName();
    int index = m_strUploadPath.lastIndexOf('/');
    QString strFileName = m_strUploadPath.right(m_strUploadPath.size()-index-1);
    QFile file(m_strUploadPath);
    qint64 iFileSize = file.size();
    PDUPtr pdu = makePDU(m_strUploadPath.size()+1);
    memcpy(pdu->caData,strFileName.toStdString().c_str(),strFileName.size());
    memcpy(pdu->caData+32,&iFileSize,sizeof(qint64));
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strUploadPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_INIT_REQUEST;
    Client::getInstance().sendMsg(pdu.get());
}
void File::on_mkDir_PB_clicked()
{
    QString fileName = QInputDialog::getText(this, "创建文件夹", "文件名").trimmed();
    if (fileName.isEmpty()) {
        qDebug() << "文件创建输入为空";
        return;
    }
    QByteArray pathBytes = m_strCurPath.toUtf8();
    QByteArray nameBytes = fileName.toUtf8();
    int pathSize = pathBytes.size();    // UTF-8字节数
    int nameSize = nameBytes.size();    // UTF-8字节数
    int sumLen = pathSize + nameSize;
    PDUPtr pdu = makePDU(sumLen);
    pdu->uiType = ENUM_MSG_TYPE_CREATE_FILE_REQUEST;
    memcpy(pdu->caData, &pathSize, sizeof(int));
    memcpy(pdu->caData + 4, &nameSize, sizeof(int));
    memcpy(pdu->caMsg, pathBytes.constData(), pathSize);
    memcpy(pdu->caMsg + pathSize, nameBytes.constData(), nameSize);
    Client::getInstance().sendMsg(pdu.get());
    qDebug() << "发送创建文件夹请求，等待响应...";
}


void File::on_flush_PB_clicked()
{
    flushfile();
}

void File::on_delFile_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    QString mPath = QString("%1/%2").arg(m_strCurPath).arg(pItem->text());
    int ret = QMessageBox::question(this,"删除文件",QString("是否删除文件 %1").arg(pItem->text()));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDUPtr pdu = makePDU(mPath.toStdString().size()+1);
    memcpy(pdu->caMsg,mPath.toStdString().c_str(),mPath.toStdString().size());
    foreach(FILE_INFO* file, m_pFileInfoList){
        if(file->name == pItem->text()){
            memcpy(pdu->caData,&file->uiType,sizeof(uint));
        }
    }
    pdu->uiType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
    Client::getInstance().sendMsg(pdu.get());
}

void File::on_rename_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    QString newFileName = QInputDialog::getText(this, "重命名文件夹", "文件名").trimmed();
    if(newFileName == ""){
        return;
    }
    PDUPtr pdu = makePDU(m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caData,pItem->text().toStdString().c_str(),32);
    memcpy(pdu->caData+32,newFileName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    Client::getInstance().sendMsg(pdu.get());
}

void File::on_move_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    QString newFilePath = QInputDialog::getText(this, "移动文件夹", "新文件路径").trimmed();
    QString oldFilePath = QString("%1/%2").arg(m_strCurPath).arg(pItem->text());
    PDUPtr pdu = makePDU(newFilePath.toStdString().size()+1);
    memcpy(pdu->caMsg,newFilePath.toStdString().c_str(),newFilePath.toStdString().size());
    memcpy(pdu->caData,oldFilePath.toStdString().c_str(),oldFilePath.toStdString().size());
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{

    foreach(FILE_INFO* file, m_pFileInfoList){
        if(file->name == item->text()){
           if(file->uiType != 0){
               return;
           }else{
               m_strCurPath = QString("%1/%2").arg(m_strCurPath).arg(item->text());
           }
        }
    }
    flushfile();
}

void File::errorBox(QString strMsg)
{
    QMessageBox::information(this,"提示",strMsg);
}

void File::on_return_PB_clicked()
{
    if(m_strCurPath == m_strIniPath){
        return;
    }
    int index = m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index,m_strCurPath.size()-index);
    flushfile();
}




void File::on_share_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    sharefile->shareFileName = pItem->text();
    sharefile->updateList();
    if(sharefile->isHidden()){
        sharefile->show();
    }
}
