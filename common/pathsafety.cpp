#include "pathsafety.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

bool isPathSafe(const QString& path, const QString& rootPath)
{
    QFileInfo fileInfo(path);
    QString canonicalPath = fileInfo.canonicalFilePath();
    if (canonicalPath.isEmpty()) {
        canonicalPath = QDir::cleanPath(fileInfo.absoluteFilePath());
    }
    QFileInfo rootInfo(rootPath);
    QString canonicalRoot = rootInfo.canonicalFilePath();
    if (canonicalRoot.isEmpty()) {
        canonicalRoot = QDir::cleanPath(rootInfo.absoluteFilePath());
    }
    if (!canonicalRoot.endsWith('/')) {
        canonicalRoot += '/';
    }
    qDebug() << "isPathSafe: path" << canonicalPath << "root" << canonicalRoot;
    return canonicalPath.startsWith(canonicalRoot) || canonicalPath == canonicalRoot.left(canonicalRoot.size() - 1);
}
