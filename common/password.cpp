#include "password.h"
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QRandomGenerator>

QByteArray pbkdf2(const QByteArray& password, const QByteArray& salt, int iterations, int keyLen)
{
    const int hashLen = 32; // SHA-256 output = 32 bytes
    QByteArray result;
    int blocks = (keyLen + hashLen - 1) / hashLen;

    for (int block = 1; block <= blocks; block++) {
        QMessageAuthenticationCode mac(QCryptographicHash::Sha256, password);
        QByteArray blockInput = salt;
        blockInput.append(static_cast<char>((block >> 24) & 0xff));
        blockInput.append(static_cast<char>((block >> 16) & 0xff));
        blockInput.append(static_cast<char>((block >> 8) & 0xff));
        blockInput.append(static_cast<char>(block & 0xff));

        mac.addData(blockInput);
        QByteArray U = mac.result();
        QByteArray T = U;

        for (int i = 1; i < iterations; i++) {
            QMessageAuthenticationCode mac2(QCryptographicHash::Sha256, password);
            mac2.addData(U);
            U = mac2.result();
            for (int j = 0; j < hashLen; j++) {
                T[j] = T[j] ^ U[j];
            }
        }
        result.append(T);
    }
    return result.left(keyLen);
}

QByteArray generateSalt()
{
    QByteArray salt(16, '\0');
    for (int i = 0; i < 16; i++) {
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    return salt.toHex(); // 32 hex chars
}

QByteArray hashPassword(const QByteArray& rawPassword, const QByteArray& hexSalt)
{
    QByteArray salt = QByteArray::fromHex(hexSalt);
    QByteArray hash = pbkdf2(rawPassword, salt, 1000, 32);
    return hash.toHex(); // 64 hex chars
}
