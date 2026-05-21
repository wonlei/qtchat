#include "operatedb.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include "../common/password.h"

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
}

OperateDB::~OperateDB()
{
    QSqlDatabase::removeDatabase(m_connBaseName);
}

QSqlDatabase OperateDB::db()
{
    if (m_threadConns.hasLocalData()) {
        return *m_threadConns.localData();
    }
    QString connName = QString("%1_%2").arg(m_connBaseName).arg(m_connCounter++);
    QSqlDatabase conn = QSqlDatabase::addDatabase("QMYSQL", connName);
    conn.setHostName(m_config.dbHost.isEmpty() ? "localhost" : m_config.dbHost);
    conn.setPort(m_config.dbPort > 0 ? m_config.dbPort : 3306);
    conn.setUserName(m_config.dbUser.isEmpty() ? "root" : m_config.dbUser);
    conn.setPassword(m_config.dbPass);
    conn.setDatabaseName(m_config.dbName.isEmpty() ? "wonchat" : m_config.dbName);
    if (!conn.open()) {
        qWarning() << "DB connection failed:" << conn.lastError().text();
    } else {
        qDebug() << "DB connected:" << connName;
    }
    QSqlDatabase* ptr = new QSqlDatabase(conn);
    m_threadConns.setLocalData(ptr);
    return conn;
}

void OperateDB::setConfig(const AppConfig& cfg)
{
    m_config = cfg;
}

void OperateDB::init()
{
    qDebug() << "Database pool initialized, connections created per thread on demand";
}

bool OperateDB::handleRegist(char *caName, char *caPwd)
{
    if (caName == NULL || caPwd == NULL) {
        return false;
    }

    QSqlQuery q(db());

    q.prepare("select * from user_info where name = ?");
    q.addBindValue(caName);
    if (!q.exec()) {
        qDebug() << "handleRegist select failed:" << q.lastError().text();
        return false;
    }
    if (q.next()) {
        qDebug() << "handleRegist: user already exists:" << caName;
        return false;
    }

    QByteArray salt = generateSalt();
    QByteArray pwdHash = hashPassword(QByteArray(caPwd), salt);

    q.prepare("insert into user_info(name, pwd, salt) values(?, ?, ?)");
    q.addBindValue(caName);
    q.addBindValue(pwdHash);
    q.addBindValue(salt);
    if (!q.exec()) {
        qDebug() << "handleRegist insert failed:" << q.lastError().text();
        return false;
    }
    qDebug() << "handleRegist success:" << caName;
    return true;
}

bool OperateDB::handleLogin(char *caName, char *caPwd)
{
    if (caName == NULL || caPwd == NULL) {
        return false;
    }

    QSqlQuery q(db());
    q.prepare("select pwd, salt from user_info where name = ?");
    q.addBindValue(caName);
    if (!q.exec()) {
        qDebug() << "handleLogin select failed:" << q.lastError().text();
        return false;
    }
    if (!q.next()) {
        qDebug() << "handleLogin: user not found:" << caName;
        return false;
    }

    QByteArray storedHash = q.value(0).toByteArray();
    QByteArray storedSalt = q.value(1).toByteArray();
    QByteArray computedHash = hashPassword(QByteArray(caPwd), storedSalt);

    if (storedHash != computedHash) {
        qDebug() << "handleLogin: password mismatch for:" << caName;
        return false;
    }

    q.prepare("update user_info set online = 1 where name = ?");
    q.addBindValue(caName);
    q.exec();
    qDebug() << "handleLogin success:" << caName;
    return true;
}

void OperateDB::clientOffLine(const char *caName)
{
    if(caName == NULL) return;

    QSqlQuery q(db());
    q.prepare("update user_info set online = 0 where name = ?");
    q.addBindValue(caName);
    q.exec();
    qDebug() << "更新用户online为0";
}

int OperateDB::handleFinduser(const char *caName)
{
    if(caName == NULL) return -1;

    QSqlQuery q(db());
    q.prepare("select online from user_info where name = ?");
    q.addBindValue(caName);
    q.exec();
    if(q.next()){
        return q.value(0).toInt();
    }
    return 2;
}

int OperateDB::handleAddFriend(const char *caCurName, const char *caTarName)
{
    if(caCurName == nullptr || caTarName == nullptr){
        return -1;
    }

    QSqlQuery q(db());
    q.prepare(R"(
        SELECT * FROM friend
        WHERE
        (
            user_id = (SELECT id FROM user_info WHERE name = ?)
            AND
            friend_id = (SELECT id FROM user_info WHERE name = ?)
        )
        OR
        (
            user_id = (SELECT id FROM user_info WHERE name = ?)
            AND
            friend_id = (SELECT id FROM user_info WHERE name = ?)
        )
    )");
    q.addBindValue(caCurName);
    q.addBindValue(caTarName);
    q.addBindValue(caTarName);
    q.addBindValue(caCurName);
    q.exec();
    if(q.next()){
        return -2;
    }

    q.prepare("select online from user_info where name = ?");
    q.addBindValue(caTarName);
    q.exec();
    if(q.next()){
        return q.value(0).toInt();
    }
    return -1;
}

QStringList OperateDB::handleOnline()
{
    QString sql = QString("select name from user_info where online = 1");
    QSqlQuery q(db());
    q.exec(sql);
    QStringList res;
    while(q.next()){
        res.append(q.value(0).toString());
    }
    return res;
}

QStringList OperateDB::handleFlush(const char* curName)
{
    QStringList res;
    if(curName == nullptr) return res;

    QSqlQuery q(db());
    q.prepare(R"(
        select name from user_info
        where id in(
            select user_id from friend where friend_id =
            (select id from user_info where name = ?)
            union
            select friend_id from friend where user_id =
            (select id from user_info where name = ?)
        ) and online = 1
    )");
    q.addBindValue(curName);
    q.addBindValue(curName);
    q.exec();
    while(q.next()) {
        res.append(q.value(0).toString());
    }
    return res;
}

bool OperateDB::handleAddFriendAgree(const char* caCurName, const char* caTarName)
{
    if(caCurName == nullptr || caTarName == nullptr){
        return false;
    }

    QSqlQuery q(db());
    q.prepare(R"(
        insert into friend(user_id, friend_id)
        select u1.id, u2.id
        from user_info u1, user_info u2
        where u1.name = ? and u2.name = ?
    )");
    q.addBindValue(caCurName);
    q.addBindValue(caTarName);
    return q.exec();
}

bool OperateDB::handleDeleteFriend(const char* caCurName, const char* caTarName)
{
    if(caCurName == nullptr || caTarName == nullptr){
        return false;
    }

    QSqlQuery q(db());
    q.prepare(R"(
        delete from friend
        where (user_id = (select id from user_info where name = ?)
               and friend_id = (select id from user_info where name = ?))
           or (user_id = (select id from user_info where name = ?)
               and friend_id = (select id from user_info where name = ?))
    )");
    q.addBindValue(caCurName);
    q.addBindValue(caTarName);
    q.addBindValue(caTarName);
    q.addBindValue(caCurName);
    return q.exec();
}

void OperateDB::saveMessage(const char *sender, const char *receiver, const char *content)
{
    if (!sender || !receiver || !content) return;
    QSqlQuery q(db());
    q.prepare("INSERT INTO message (sender, receiver, content) VALUES (?, ?, ?)");
    q.addBindValue(sender);
    q.addBindValue(receiver);
    q.addBindValue(content);
    q.exec();
}

QByteArray OperateDB::getChatHistory(const char *user1, const char *user2)
{
    if (!user1 || !user2) return QByteArray();

    QSqlQuery q(db());
    q.prepare(R"(
        SELECT sender, content FROM message
        WHERE (sender = ? AND receiver = ?) OR (sender = ? AND receiver = ?)
        ORDER BY created_at ASC LIMIT 100
    )");
    q.addBindValue(user1);
    q.addBindValue(user2);
    q.addBindValue(user2);
    q.addBindValue(user1);
    q.exec();

    struct {
        QByteArray sender;
        QByteArray content;
    } rows[100];
    int count = 0;

    while (q.next() && count < 100) {
        rows[count].sender = q.value(0).toByteArray();
        rows[count].content = q.value(1).toByteArray();
        count++;
    }

    int bodySize = 0;
    for (int i = 0; i < count; i++) {
        bodySize += 32 + sizeof(uint) + rows[i].content.size();
    }

    QByteArray result(sizeof(int) + bodySize, '\0');
    memcpy(result.data(), &count, sizeof(int));
    int offset = sizeof(int);
    for (int i = 0; i < count; i++) {
        char senderBuf[32] = {'\0'};
        strncpy(senderBuf, rows[i].sender.constData(), 31);
        memcpy(result.data() + offset, senderBuf, 32);
        offset += 32;
        uint contentLen = rows[i].content.size();
        memcpy(result.data() + offset, &contentLen, sizeof(uint));
        offset += sizeof(uint);
        memcpy(result.data() + offset, rows[i].content.constData(), contentLen);
        offset += contentLen;
    }

    return result;
}

bool OperateDB::isUserOnline(const char *name)
{
    if (!name) return false;
    QSqlQuery q(db());
    q.prepare("SELECT online FROM user_info WHERE name = ?");
    q.addBindValue(name);
    if (q.exec() && q.next()) {
        return q.value(0).toInt() == 1;
    }
    return false;
}

void OperateDB::saveOfflineMessage(const char *sender, const char *receiver, const char *content)
{
    if (!sender || !receiver || !content) return;
    QSqlQuery q(db());
    q.prepare("INSERT INTO offline_message (sender, receiver, content) VALUES (?, ?, ?)");
    q.addBindValue(sender);
    q.addBindValue(receiver);
    q.addBindValue(content);
    q.exec();
}

QList<QPair<QString,QString>> OperateDB::getOfflineMessages(const char *receiver)
{
    QList<QPair<QString,QString>> result;
    if (!receiver) return result;
    QSqlQuery q(db());
    q.prepare("SELECT sender, content, id FROM offline_message WHERE receiver = ? ORDER BY created_at ASC LIMIT 50");
    q.addBindValue(receiver);
    if (!q.exec()) return result;
    while (q.next()) {
        result.append({q.value(0).toString(), q.value(1).toString()});
    }
    return result;
}

void OperateDB::clearOfflineMessages(const char *receiver)
{
    if (!receiver) return;
    QSqlQuery q(db());
    q.prepare("DELETE FROM offline_message WHERE receiver = ?");
    q.addBindValue(receiver);
    q.exec();
}
