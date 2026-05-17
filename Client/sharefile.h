#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>

namespace Ui {
class ShareFile;
}

class ShareFile : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFile(QWidget *parent = nullptr);
    ~ShareFile();
    void updateList();
    QString shareFileName;
private slots:
    void on_allSelect_pb_clicked();
    void on_cancelSelect_pb_clicked();

    void on_ok_pb_clicked();

private:
    Ui::ShareFile *ui;
};

#endif // SHAREFILE_H
