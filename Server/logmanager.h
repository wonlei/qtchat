#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QString>
#include <QFile>
#include <QMutex>
#include <QDateTime>

class LogManager
{
public:
    enum Level { DEBUG, INFO, WARN, ERROR };

    static void init(const QString& dir = "logs", const QString& prefix = "server");
    static void cleanup();

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg);
    static QString levelToString(int level);
    static void ensureFileOpen();

    static QFile s_file;
    static QMutex s_mutex;
    static QString s_dir;
    static QString s_prefix;
    static QDate s_currentDate;
};

#define LOG_DEBUG()    qDebug()
#define LOG_INFO()     qInfo()
#define LOG_WARN()     qWarning()
#define LOG_ERROR()    qCritical()

#endif // LOGMANAGER_H
