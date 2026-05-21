#include "config.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

bool AppConfig::loadFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Config: cannot open" << path << ", trying resource";
        return loadFromResource(path);
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Config: JSON parse error" << err.errorString();
        return false;
    }

    QJsonObject root = doc.object();

    if (root.contains("server")) {
        QJsonObject srv = root["server"].toObject();
        serverIp   = srv.value("ip").toString(serverIp);
        serverPort = static_cast<quint16>(srv.value("port").toInt(serverPort));
        useTls     = srv.value("tls").toBool(useTls);
    }
    if (root.contains("storage")) {
        QJsonObject stg = root["storage"].toObject();
        rootPath = stg.value("root_path").toString(rootPath);
    }
    if (root.contains("database")) {
        QJsonObject db = root["database"].toObject();
        dbHost = db.value("host").toString(dbHost);
        dbPort = db.value("port").toInt(dbPort);
        dbName = db.value("name").toString(dbName);
        dbUser = db.value("user").toString(dbUser);
        dbPass = db.value("password").toString(dbPass);
    }

    qDebug() << "Config loaded from" << path;
    return true;
}

bool AppConfig::loadFromResource(const QString& path)
{
    QFile file(":" + path.mid(path.lastIndexOf('/') + 1));
    if (!file.open(QIODevice::ReadOnly))
        return false;
    return loadFromFile(":" + path.mid(path.lastIndexOf('/') + 1));
}

void AppConfig::loadLegacy(const QString& text)
{
    QStringList slist = text.split(QRegExp("[\r\n]+"), QString::SkipEmptyParts);
    if (slist.size() >= 1) serverIp = slist[0];
    if (slist.size() >= 2) serverPort = slist[1].toUShort();
    if (slist.size() >= 3) rootPath = slist[2];
    if (slist.size() >= 4) useTls = (slist[3].toInt() == 1);
}
