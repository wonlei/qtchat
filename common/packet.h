#ifndef PACKET_H
#define PACKET_H

#include "protocol.h"
#include <cstdint>
#include <memory>
#include <QByteArray>

// Frame wire format:
//   [4 bytes: total_len][1 byte: version][2 bytes: msg_type]
//   [4 bytes: meta_len][4 bytes: payload_len]
//   [meta_len bytes: meta][payload_len bytes: payload]
// Total header = 15 bytes

struct Frame {
    static constexpr uint8_t  CURRENT_VERSION = 2;
    static constexpr uint16_t HEADER_SIZE     = 15;
    static constexpr uint32_t MAGIC_MASK      = 0x80000000; // high bit of total_len = new format marker

    uint8_t  version = CURRENT_VERSION;
    uint16_t msg_type = 0;
    QByteArray meta;
    QByteArray payload;

    // Serialize to wire format
    QByteArray serialize() const;

    // Deserialize from wire data. Returns nullptr on failure.
    static std::unique_ptr<Frame> deserialize(const QByteArray& data);

    // Auto-detect format: true = new Frame, false = old PDU
    static bool isFrameData(const QByteArray& data);
};

using FramePtr = std::unique_ptr<Frame>;

inline FramePtr makeFrame(uint16_t msgType, const QByteArray& meta = {}, const QByteArray& payload = {}) {
    auto f = std::make_unique<Frame>();
    f->msg_type = msgType;
    f->meta = meta;
    f->payload = payload;
    return f;
}

#endif // PACKET_H
