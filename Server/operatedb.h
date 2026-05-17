 #ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <qsqldatabase.h>

class OperateDB : public QObject
{
    Q_OBJECT
public:
    QSqlDatabase m_db;
    ~OperateDB();
    void connect();
    static OperateDB &getInstance();
    bool handleRegist(char* caName,char* caPwd);
    bool handleLogin(char* caName,char* caPwd);
    void clientOffLine(const char* caName);
    int handleFinduser(const char* caName);
    int handleAddFriend(const char* caCurName,const char* caTarName);
    QStringList handleOnline();
    QStringList handleFlush(const char* curName);
    bool handleAddFriendAgree(const char* caCurName,const char* caTarName);
    bool handleDeleteFriend(const char* caCurName,const char* caTarName);
    void saveMessage(const char* sender, const char* receiver, const char* content);
    QByteArray getChatHistory(const char* user1, const char* user2);
private:
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& instance) = delete;
    OperateDB operator=(const OperateDB&) = delete;
signals:

};

#endif // OPERATEDB_H
