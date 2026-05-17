#ifndef FILE_H
#define FILE_H

#include <QWidget>
#include <qlistwidget.h>
#include"../common/protocol.h"
#include "sharefile.h"

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    QString m_strCurPath;
    QString m_strIniPath;
    QString m_strUploadPath;
    ShareFile* sharefile;
    explicit File(QWidget *parent = nullptr);
    void flushfile();
    QList<FILE_INFO*>m_pFileInfoList;
    void updateFileList(QList<FILE_INFO *> pFileList);
    ~File();
    void uploadFile();
private slots:
    void on_upload_PB_clicked();

    void on_mkDir_PB_clicked();

    void on_return_PB_clicked();

    void on_flush_PB_clicked();

    void on_delFile_PB_clicked();

    void on_rename_PB_clicked();

    void on_move_PB_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void errorBox(QString strMsg);

    void on_share_PB_clicked();

private:
    Ui::File *ui;
};

#endif // FILE_H
