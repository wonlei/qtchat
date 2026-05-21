#include "logmanager.h"
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QThread>
#include <iostream>

QFile   LogManager::s_file;
QMutex  LogManager::s_mutex;
QString LogManager::s_dir;
QString LogManager::s_prefix;
QDate   LogManager::s_currentDate;

void LogManager::init(const QString& dir, const QString& prefix)
{
    s_dir = dir;
    s_prefix = prefix;

    QDir().mkpath(s_dir);

    qInstallMessageHandler(messageHandler);
    qInfo() << "=== LogManager initialized ===";
}

void LogManager::cleanup()
{
    if (s_file.isOpen()) {
        qInfo() << "=== LogManager shutting down ===";
        s_file.close();
    }
    qInstallMessageHandler(nullptr);
}

void LogManager::messageHandler(QtMsgType type, const QMessageLogContext& /*ctx*/, const QString& msg)
{
    QMutexLocker locker(&s_mutex);

    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString threadName = QThread::currentThread()->objectName();
    if (threadName.isEmpty())
        threadName = QString("0x%1").arg((quintptr)QThread::currentThreadId(), 0, 16);

    const char* levelStr;
    switch (type) {
    case QtDebugMsg:    levelStr = "DEBUG"; break;
    case QtInfoMsg:     levelStr = "INFO "; break;
    case QtWarningMsg:  levelStr = "WARN "; break;
    case QtCriticalMsg: levelStr = "ERROR"; break;
    case QtFatalMsg:    levelStr = "FATAL"; break;
    default:            levelStr = "DEBUG"; break;
    }

    QString line = QString("[%1] [%2] [%3] %4\n")
        .arg(timestamp, levelStr, threadName, msg);

    // stdout
    std::cout << line.toStdString();
    std::cout.flush();

    // file (with daily rotation)
    ensureFileOpen();
    if (s_file.isOpen()) {
        s_file.write(line.toUtf8());
        s_file.flush();
    }
}

QString LogManager::levelToString(int level)
{
    switch (level) {
    case DEBUG: return "DEBUG";
    case INFO:  return "INFO";
    case WARN:  return "WARN";
    case ERROR: return "ERROR";
    default:    return "UNKNOWN";
    }
}

void LogManager::ensureFileOpen()
{
    QDate today = QDate::currentDate();
    if (s_file.isOpen() && s_currentDate == today)
        return;

    if (s_file.isOpen())
        s_file.close();

    s_currentDate = today;
    QString filename = QString("%1/%2_%3.log")
        .arg(s_dir, s_prefix, today.toString("yyyy-MM-dd"));
    s_file.setFileName(filename);
    s_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}
