#include "operatedb.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}

OperateDB::~OperateDB()
{
    m_db.close();
}

void OperateDB::connect()
{
    m_db.setHostName(qEnvironmentVariable("DB_HOST", "localhost"));
    int port = qEnvironmentVariableIntValue("DB_PORT");
    m_db.setPort(port > 0 ? port : 3306);
    m_db.setUserName(qEnvironmentVariable("DB_USER", "root"));
    m_db.setPassword(qEnvironmentVariable("DB_PASS", "200220"));
    m_db.setDatabaseName(qEnvironmentVariable("DB_NAME", "mydb2503"));
    if(m_db.open()){
        qDebug()<<"连接数据库成功";
    }else{
        qDebug()<<"数据库连接失败"<<m_db.lastError().text();
    }
}

OperateDB &OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}

bool OperateDB::handleRegist(char *caName, char *caPwd)
{
    if (caName == NULL || caPwd == NULL) {
        return false;
    }

    QSqlQuery q;

    // 检查用户是否存在
    q.prepare("select * from user_info where name = ?");
    q.addBindValue(caName);
    if (!q.exec()) {
        return false;
    }
    if (q.next()) {
        return false; // 用户已存在
    }

    // 插入新用户
    q.prepare("insert into user_info(name, pwd) values(?, ?)");
    q.addBindValue(caName);
    q.addBindValue(caPwd);
    return q.exec();
}

bool OperateDB::handleLogin(char *caName, char *caPwd)
{
    if (caName == NULL || caPwd == NULL) {
        return false;
    }

    QSqlQuery q;
    q.prepare("select * from user_info where name = ? and pwd = ?");
    q.addBindValue(caName);
    q.addBindValue(caPwd);
    if (q.exec() && q.next()) {
        q.prepare("update user_info set online = 1 where name = ? and pwd = ?");
        q.addBindValue(caName);
        q.addBindValue(caPwd);
        q.exec();
        return true;
    }
    return false;
}

void OperateDB::clientOffLine(const char *caName)
{
    if(caName == NULL) return;

    QSqlQuery q;
    q.prepare("update user_info set online = 0 where name = ?");
    q.addBindValue(caName);
    q.exec();
    qDebug() << "更新用户online为0";
}

int OperateDB::handleFinduser(const char *caName)
{
    if(caName == NULL) return -1;

    QSqlQuery q;
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

    QSqlQuery q;
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
    QSqlQuery q;
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

    QSqlQuery q;
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

    QSqlQuery q;
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

    QSqlQuery q;
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
    QSqlQuery q;
    q.prepare("INSERT INTO message (sender, receiver, content) VALUES (?, ?, ?)");
    q.addBindValue(sender);
    q.addBindValue(receiver);
    q.addBindValue(content);
    q.exec();
}

QByteArray OperateDB::getChatHistory(const char *user1, const char *user2)
{
    if (!user1 || !user2) return QByteArray();

    QSqlQuery q;
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
