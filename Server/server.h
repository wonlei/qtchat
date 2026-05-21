#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>

class OperateDB;

class Server : public QMainWindow
{


public:
    ~Server();
    void loadConfig();
    static Server& getInstance();
    QString m_strlIP;
    quint16 m_usPort;
    QString m_strRootPath;
    OperateDB* m_db = nullptr;
private:
    explicit Server(QWidget *parent = nullptr);
    Server(const Server& instance) = delete;
    Server operator=(const Server&) = delete;
};
#endif // SERVER_H
