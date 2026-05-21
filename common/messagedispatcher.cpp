#include "messagedispatcher.h"

void MessageDispatcher::registerHandler(uint msgType, MessageHandler handler)
{
    m_handlers[msgType] = std::move(handler);
}

PDUPtr MessageDispatcher::dispatch(PDU* pdu) const
{
    if (!pdu) return nullptr;

    auto it = m_handlers.find(pdu->uiType);
    if (it != m_handlers.end()) {
        return it->second(pdu);
    }
    return nullptr;
}
