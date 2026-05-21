#include "protocol.h"
#include <QDebug>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
PDU *mkPDU(uint uiMsgLen)
{
    uint uiTotalLen = sizeof(PDU) + uiMsgLen;
    PDU* pdu = (PDU*)malloc(uiTotalLen);
    if (pdu == nullptr) {
        qCritical() << "mkPDU: malloc failed for size" << uiTotalLen;
        return nullptr;
    }
    memset(pdu, 0, uiTotalLen);
    pdu->uiTotalLen = uiTotalLen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}
