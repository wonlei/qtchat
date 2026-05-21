#ifndef PASSWORD_H
#define PASSWORD_H

#include <QByteArray>

QByteArray pbkdf2(const QByteArray& password, const QByteArray& salt, int iterations = 1000, int keyLen = 32);
QByteArray generateSalt();
QByteArray hashPassword(const QByteArray& rawPassword, const QByteArray& hexSalt);

#endif // PASSWORD_H
