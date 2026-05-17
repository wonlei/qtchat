#ifndef UPLOADER_H
#define UPLOADER_H

#include "../common/protocol.h"

#include <QObject>

class Uploader : public QObject
{
    Q_OBJECT
public: 
    QString m_strUploadPath;
    Uploader(QString strFilePath);
    void start();
public slots:
    void UploadFile();

signals:
    void errorSignal(const QString strMsg);
    void finished();
    void uploadPDU(PDU* pdu);
};

#endif // UPLOADER_H
