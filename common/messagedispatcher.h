#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#include "../common/protocol.h"
#include <functional>
#include <unordered_map>

// Handler signature: takes PDU*, returns PDUPtr (or nullptr to send nothing back)
using MessageHandler = std::function<PDUPtr(PDU*)>;

class MessageDispatcher
{
public:
    void registerHandler(uint msgType, MessageHandler handler);
    PDUPtr dispatch(PDU* pdu) const;

private:
    std::unordered_map<uint, MessageHandler> m_handlers;
};

#endif // MESSAGEDISPATCHER_H
