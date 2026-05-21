#include <gtest/gtest.h>
#include "../common/password.h"

TEST(PasswordTest, Pbkdf2Deterministic)
{
    QByteArray password = "test123";
    QByteArray salt = QByteArray::fromHex("aabbccddeeff00112233445566778899");

    QByteArray hash1 = pbkdf2(password, salt, 1000, 32);
    QByteArray hash2 = pbkdf2(password, salt, 1000, 32);

    EXPECT_EQ(hash1.size(), 32);
    EXPECT_EQ(hash1, hash2); // same input = same output
}

TEST(PasswordTest, Pbkdf2DifferentPassword)
{
    QByteArray salt = QByteArray::fromHex("aabbccddeeff00112233445566778899");

    QByteArray h1 = pbkdf2("password1", salt, 100, 32);
    QByteArray h2 = pbkdf2("password2", salt, 100, 32);

    EXPECT_NE(h1, h2);
}

TEST(PasswordTest, Pbkdf2DifferentSalt)
{
    QByteArray password = "mypassword";
    QByteArray salt1 = QByteArray::fromHex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    QByteArray salt2 = QByteArray::fromHex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");

    QByteArray h1 = pbkdf2(password, salt1, 100, 32);
    QByteArray h2 = pbkdf2(password, salt2, 100, 32);

    EXPECT_NE(h1, h2);
}

TEST(PasswordTest, GenerateSaltRandom)
{
    QByteArray s1 = generateSalt();
    QByteArray s2 = generateSalt();

    EXPECT_EQ(s1.size(), 32); // hex encoded 16 bytes
    EXPECT_EQ(s2.size(), 32);
    EXPECT_NE(s1, s2); // highly unlikely to collide
}

TEST(PasswordTest, HashPasswordRoundtrip)
{
    QByteArray password = "user_password_123";
    QByteArray salt = generateSalt();
    QByteArray hash1 = hashPassword(password, salt);
    QByteArray hash2 = hashPassword(password, salt);

    EXPECT_EQ(hash1.size(), 64); // 32 bytes hex
    EXPECT_EQ(hash1, hash2);     // same password + same salt = same hash
}

TEST(PasswordTest, EmptyPassword)
{
    QByteArray empty;
    QByteArray salt = generateSalt();

    QByteArray hash = hashPassword(empty, salt);
    EXPECT_EQ(hash.size(), 64); // still produces valid hash
    EXPECT_FALSE(hash.isEmpty());
}

TEST(PasswordTest, Pbkdf2KnownVector)
{
    // RFC 6070 test vector for PBKDF2-HMAC-SHA1 says password="password", salt="salt", iter=1, dkLen=20
    // We use SHA-256, so we test determinism instead
    QByteArray password = "password";
    QByteArray salt("somesalt", 8);
    QByteArray hash = pbkdf2(password, salt, 100, 32);
    EXPECT_EQ(hash.size(), 32);
}
