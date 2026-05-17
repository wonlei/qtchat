#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include <QWidget>
#include <qlistwidget.h>

namespace Ui {
class Onlineuser;
}

class Onlineuser : public QWidget
{
    Q_OBJECT

public:
    explicit Onlineuser(QWidget *parent = nullptr);
    ~Onlineuser();
    void updateOnlineUser(QStringList nameList);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::Onlineuser *ui;
};

#endif // ONLINEUSER_H
