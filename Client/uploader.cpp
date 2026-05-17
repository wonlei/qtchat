#include "uploader.h"

#include <QThread>
#include <qfile.h>


Uploader::Uploader(QString strFilePath)
{
    m_strUploadPath = strFilePath;
}

void Uploader::start()
{
    QThread* thread = new QThread;
    connect(thread,&QThread::started,this,&Uploader::UploadFile,Qt::QueuedConnection);
    //进程发送开始的信号，这个进程进行上传文件函数，以消息队列进行；
    connect(this,&Uploader::finished,thread,&QThread::quit,Qt::QueuedConnection);
    //这个进程发送结束信号，进程以结束停止回应，以消息队列进行；
    connect(this,&Uploader::finished,thread,&QThread::deleteLater,Qt::QueuedConnection);
    //这个进程发送结束信号，进程以销毁做回应，以消息对列进行
    thread->start();
}

void Uploader::UploadFile()
{
    QFile file(m_strUploadPath);
    if(!file.open(QIODevice::ReadOnly)){
        emit errorSignal("文件上传失败");
        emit finished();
        return;
    }
    while(1){
        PDU* pdu = mkPDU(4096);
        pdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_DEAL_REQUEST;
        int ret = file.read(pdu->caMsg,4096);
        if(ret<0){
            emit errorSignal("文件上传失败");
            return;
        }
        if(ret == 0){
            break;
        }
        pdu->uiMsgLen = ret;
        pdu->uiTotalLen = sizeof(PDU) + ret;
        emit uploadPDU(pdu);
    }
    file.close();
    emit finished();
}
