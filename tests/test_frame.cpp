#include <gtest/gtest.h>
#include <QString>
#include "../common/packet.h"
#include "../common/protocol.h"

TEST(FrameTest, SerializeDeserializeRoundtrip)
{
    QByteArray meta(64, 'A');
    QByteArray payload(128, 'B');

    Frame f;
    f.msg_type = ENUM_MSG_TYPE_CHAT_REQUEST;
    f.meta = meta;
    f.payload = payload;

    QByteArray wire = f.serialize();

    // Verify wire format starts with magic marker
    uint32_t firstWord;
    memcpy(&firstWord, wire.constData(), 4);
    EXPECT_TRUE(firstWord & Frame::MAGIC_MASK);

    // Deserialize
    auto f2 = Frame::deserialize(wire);
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f2->version, Frame::CURRENT_VERSION);
    EXPECT_EQ(f2->msg_type, ENUM_MSG_TYPE_CHAT_REQUEST);
    EXPECT_EQ(f2->meta.size(), 64);
    EXPECT_EQ(f2->payload.size(), 128);
    EXPECT_EQ(f2->meta, meta);
    EXPECT_EQ(f2->payload, payload);
}

TEST(FrameTest, EmptyMetaAndPayload)
{
    Frame f;
    f.msg_type = ENUM_MSG_TYPE_HEARTBEAT_REQUEST;
    QByteArray wire = f.serialize();

    auto f2 = Frame::deserialize(wire);
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f2->msg_type, ENUM_MSG_TYPE_HEARTBEAT_REQUEST);
    EXPECT_EQ(f2->meta.size(), 0);
    EXPECT_EQ(f2->payload.size(), 0);
}

TEST(FrameTest, IsFrameDataDetection)
{
    // New Frame has magic marker (high bit set)
    Frame f;
    f.msg_type = 1;
    QByteArray wire = f.serialize();

    // PDU simulation: first 4 bytes = sizeof(PDU) without magic bit
    PDU pdu;
    pdu.uiTotalLen = sizeof(PDU);
    pdu.uiMsgLen = 0;
    pdu.uiType = 0;
    memset(pdu.caData, 0, 64);
    QByteArray pduData(reinterpret_cast<char*>(&pdu), sizeof(PDU));

    EXPECT_TRUE(Frame::isFrameData(wire));
    EXPECT_FALSE(Frame::isFrameData(pduData));
}

TEST(FrameTest, DeserializeInvalidData)
{
    // Too small
    QByteArray small(5, '\0');
    EXPECT_EQ(Frame::deserialize(small), nullptr);

    // Valid magic but incomplete
    Frame f;
    f.meta = QByteArray(100, 'X');
    QByteArray wire = f.serialize();
    wire.truncate(10); // incomplete
    EXPECT_EQ(Frame::deserialize(wire), nullptr);
}

TEST(FrameTest, UnicodeRoundtrip)
{
    QByteArray meta(64, '\0');
    QString chinese = "你好，世界！";
    QByteArray chineseUtf8 = chinese.toUtf8();
    memcpy(meta.data(), chineseUtf8.constData(), qMin(64, chineseUtf8.size()));

    Frame f;
    f.msg_type = ENUM_MSG_TYPE_CHAT_REQUEST;
    f.meta = meta;
    f.payload = "hello";

    QByteArray wire = f.serialize();
    auto f2 = Frame::deserialize(wire);
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f2->meta, meta);
}
