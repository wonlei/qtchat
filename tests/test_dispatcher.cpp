#include <gtest/gtest.h>
#include "../common/messagedispatcher.h"
#include "../common/protocol.h"

TEST(DispatcherTest, RegisterAndDispatch)
{
    MessageDispatcher disp;
    bool called = false;

    disp.registerHandler(ENUM_MSG_TYPE_CREATE_FILE_REQUEST, [&called](PDU* p) {
        called = true;
        return nullptr;
    });

    PDUPtr pdu = makePDU(0);
    pdu->uiType = ENUM_MSG_TYPE_CREATE_FILE_REQUEST;

    disp.dispatch(pdu.get());
    EXPECT_TRUE(called);
}

TEST(DispatcherTest, DispatchNonexistentHandler)
{
    MessageDispatcher disp;
    PDUPtr pdu = makePDU(0);
    pdu->uiType = ENUM_MSG_TYPE_MAX; // unregistered

    auto result = disp.dispatch(pdu.get());
    EXPECT_EQ(result, nullptr);
}

TEST(DispatcherTest, MultipleHandlers)
{
    MessageDispatcher disp;
    int count = 0;

    disp.registerHandler(ENUM_MSG_TYPE_REGISTER_REQUEST, [&count](PDU*) {
        count++; return nullptr; });
    disp.registerHandler(ENUM_MSG_TYPE_LOGIN_REQUEST, [&count](PDU*) {
        count++; return nullptr; });
    disp.registerHandler(ENUM_MSG_TYPE_CHAT_REQUEST, [&count](PDU*) {
        count++; return nullptr; });

    PDUPtr pdu = makePDU(0);

    pdu->uiType = ENUM_MSG_TYPE_REGISTER_REQUEST;
    disp.dispatch(pdu.get());
    EXPECT_EQ(count, 1);

    pdu->uiType = ENUM_MSG_TYPE_LOGIN_REQUEST;
    disp.dispatch(pdu.get());
    EXPECT_EQ(count, 2);

    pdu->uiType = ENUM_MSG_TYPE_CHAT_REQUEST;
    disp.dispatch(pdu.get());
    EXPECT_EQ(count, 3);
}

TEST(DispatcherTest, HandlerReturnsResponse)
{
    MessageDispatcher disp;
    const char* testMsg = "hello";

    disp.registerHandler(ENUM_MSG_TYPE_CHAT_REQUEST, [testMsg](PDU*) {
        PDUPtr resp = makePDU(10);
        memcpy(resp->caMsg, testMsg, 6);
        resp->uiType = ENUM_MSG_TYPE_CHAT_RESPOND;
        return resp;
    });

    PDUPtr pdu = makePDU(0);
    pdu->uiType = ENUM_MSG_TYPE_CHAT_REQUEST;

    auto resp = disp.dispatch(pdu.get());
    ASSERT_NE(resp, nullptr);
    EXPECT_EQ(resp->uiType, ENUM_MSG_TYPE_CHAT_RESPOND);
    EXPECT_STREQ(resp->caMsg, "hello");
}

TEST(DispatcherTest, OverwriteHandler)
{
    MessageDispatcher disp;
    int first = 0, second = 0;

    disp.registerHandler(1, [&first](PDU*) { first++; return nullptr; });
    disp.registerHandler(1, [&second](PDU*) { second++; return nullptr; });

    PDUPtr pdu = makePDU(0);
    pdu->uiType = 1;
    disp.dispatch(pdu.get());

    EXPECT_EQ(first, 0);   // first handler overwritten
    EXPECT_EQ(second, 1);
}
