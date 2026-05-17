#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
PDU *mkPDU(uint uiMsgLen)
{
    uint uiTotalLen = sizeof(PDU) + uiMsgLen;
    PDU* pdu =(PDU*)malloc(uiTotalLen);
    if(pdu == nullptr){
        exit(1);
    }
    memset(pdu, 0 , uiTotalLen);
    pdu->uiTotalLen = uiTotalLen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}
