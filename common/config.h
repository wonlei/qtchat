#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QJsonObject>

struct AppConfig {
    // Server settings
    QString serverIp   = "127.0.0.1";
    quint16 serverPort = 5000;
    bool    useTls     = false;

    // Storage
    QString rootPath = "./fliesys";

    // Database
    QString dbHost = "localhost";
    int     dbPort = 3306;
    QString dbName = "wonchat";
    QString dbUser = "root";
    QString dbPass = "";

    bool loadFromFile(const QString& path);
    bool loadFromResource(const QString& path);
    void loadLegacy(const QString& text);
};

#endif // CONFIG_H
