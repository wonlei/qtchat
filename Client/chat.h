#ifndef CHAT_H
#define CHAT_H

#include "../common/protocol.h"

#include <QWidget>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();
    QString m_strChatName;
    void updateShow_TE(QString Msg);
    void loadHistory(PDU* pdu);
    void requestHistory();
private slots:
    void on_send_PB_clicked();


private:
    Ui::Chat *ui;
};

#endif // CHAT_H
