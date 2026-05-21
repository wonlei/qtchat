#include "packet.h"
#include <cstring>

QByteArray Frame::serialize() const
{
    uint32_t total_len = HEADER_SIZE + meta.size() + payload.size();
    QByteArray data(total_len, '\0');

    uint8_t* p = reinterpret_cast<uint8_t*>(data.data());

    // total_len with magic marker (high bit set to distinguish from old PDU)
    uint32_t wire_len = total_len | MAGIC_MASK;
    std::memcpy(p, &wire_len, 4);       p += 4;
    *p++ = version;
    std::memcpy(p, &msg_type, 2);       p += 2;
    uint32_t meta_len = meta.size();
    std::memcpy(p, &meta_len, 4);       p += 4;
    uint32_t payload_len = payload.size();
    std::memcpy(p, &payload_len, 4);    p += 4;

    if (meta_len > 0)
        std::memcpy(p, meta.constData(), meta_len);
    if (payload_len > 0)
        std::memcpy(p + meta_len, payload.constData(), payload_len);

    return data;
}

std::unique_ptr<Frame> Frame::deserialize(const QByteArray& data)
{
    if (static_cast<size_t>(data.size()) < HEADER_SIZE)
        return nullptr;

    const uint8_t* p = reinterpret_cast<const uint8_t*>(data.constData());

    uint32_t total_len;
    std::memcpy(&total_len, p, 4);      p += 4;
    total_len &= ~MAGIC_MASK;
    uint8_t  version   = *p++;
    uint16_t msg_type;
    std::memcpy(&msg_type, p, 2);       p += 2;
    uint32_t meta_len;
    std::memcpy(&meta_len, p, 4);       p += 4;
    uint32_t payload_len;
    std::memcpy(&payload_len, p, 4);    p += 4;

    if (data.size() < static_cast<int>(total_len))
        return nullptr;

    auto f = std::make_unique<Frame>();
    f->version  = version;
    f->msg_type = msg_type;
    f->meta     = QByteArray(reinterpret_cast<const char*>(p), meta_len);
    p += meta_len;
    f->payload  = QByteArray(reinterpret_cast<const char*>(p), payload_len);

    return f;
}

bool Frame::isFrameData(const QByteArray& data)
{
    if (data.size() < 4) return false;
    uint32_t first_word;
    std::memcpy(&first_word, data.constData(), 4);
    // High bit set = new Frame format. Old PDU's uiTotalLen is always < 2GB
    return (first_word & MAGIC_MASK) != 0;
}
