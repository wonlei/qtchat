#ifndef CLIENT_H
#define CLIENT_H


#include "../common/protocol.h"
#include "../common/messagedispatcher.h"

#include <QMainWindow>
#include <QSslSocket>
#include <QTimer>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    enum class ConnectionState { Disconnected, Connecting, Connected, LoggedIn };

    QString m_strLoginName;
    void recvMsg();
    void loadConfig();
    ~Client();
    QString m_strlIP;
    quint16 m_usPort;
    QString m_strRootPath;
    QByteArray buffer;
    bool m_bRecving = false;
    static Client& getInstance();
    QSslSocket socket;
    MessageDispatcher m_dispatcher;
    bool m_bUseTls = false;
    QTimer* m_heartbeatTimer;
    ConnectionState state() const { return m_state; }
    void setLoggedIn() { m_state = ConnectionState::LoggedIn; }

    void handleMsg(PDU* pdu);
    void sendHeartbeat();
public slots:
    void showConnect();
    void sendMsg(const PDU* pdu);
private slots:

    void on_regist_PB_clicked();

    void on_login_PB_clicked();

private:
    Client(QWidget *parent = nullptr);
    Client(const Client& instance) = delete;
    Client& operator=(const Client&) = delete;
    Ui::Client *ui;
    ConnectionState m_state = ConnectionState::Disconnected;
};
#endif // CLIENT_H
