#ifndef OPERATEDB_H
#define OPERATEDB_H

#include "../common/config.h"
#include <QObject>
#include <QSqlDatabase>
#include <QThreadStorage>

class OperateDB : public QObject
{
    Q_OBJECT
public:
    explicit OperateDB(QObject *parent = nullptr);
    ~OperateDB();
    void setConfig(const AppConfig& cfg);
    void init();
    QSqlDatabase db();
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
    // Offline messages
    bool isUserOnline(const char* name);
    void saveOfflineMessage(const char* sender, const char* receiver, const char* content);
    QList<QPair<QString,QString>> getOfflineMessages(const char* receiver);
    void clearOfflineMessages(const char* receiver);
private:
    OperateDB(const OperateDB& instance) = delete;
    OperateDB operator=(const OperateDB&) = delete;
    QThreadStorage<QSqlDatabase*> m_threadConns;
    QString m_connBaseName = "wonchat_conn";
    int m_connCounter = 0;
    AppConfig m_config;
signals:

};

#endif // OPERATEDB_H
