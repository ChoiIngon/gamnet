#include "Cheat.h"
#include "../Component/UserData/Item.h"
#include "../Component/UserData.h"
#include "../Util/Transaction.h"
#include "../../idl/MessageUser.h"

void Cheat::Init()
{
    Register("AddItem", std::bind(&Cheat::AddItem, this, std::placeholders::_1, std::placeholders::_2));
}

void Cheat::Register(const std::string& command, const Handler& handler)
{
    handlers.insert(std::make_pair(command, handler));
}

int Cheat::Command(const std::string& command, std::shared_ptr<UserSession> session, const std::list<Variant>& params)
{
    auto itr = handlers.find(command);
    if (handlers.end() == itr)
    {
        return (int)Message::ErrorCode::UndefineError;
    }

    return itr->second(session, params);
}

int Cheat::AddItem(const std::shared_ptr<UserSession>& session, const std::list<Variant>& params)
{
    auto itr = params.begin();
    const Variant& vIndex = *itr++;
    const Variant& vCount = *itr++;

    int index = (int)vIndex;
    int count = (int)vCount;

    auto pItemData = Item::Create(index, count);
    if (nullptr == pItemData)
    {
        return 0;
    }

    Transaction transaction(session);
    if (false == transaction(Item::InsertIntoBag(session, pItemData)))
    {
        transaction.Rollback();
        return 0;
    }

    transaction.Commit();
    return 0;
}

void Test_CheatItem_Req(const std::shared_ptr<TestSession>& session)
{
    Message::User::MsgCliSvr_Cheat_Req req;
    req.command = "AddItem";

    if (TestSession::item_index.size() > session->cheat_item_order)
    {
        const std::string item_index = std::to_string(TestSession::item_index[session->cheat_item_order]);
        const std::string item_count = "1";
        req.params.push_back(item_index);
        req.params.push_back(item_count);
    }
    
    Gamnet::Test::SendMsg(session, req);
}

void Test_CheatItem_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
    session->cheat_item_order++;

    Message::User::MsgCliSvr_Cheat_Req req;
    req.command = "AddItem";
    if (TestSession::item_index.size() > session->cheat_item_order)
    {
        const std::string item_index = std::to_string(TestSession::item_index[session->cheat_item_order]);
        const std::string item_count = "1";
        req.params.push_back(item_index);
        req.params.push_back(item_count);
        Gamnet::Test::SendMsg(session, req);
        return;
    }

    session->Next();
}

GAMNET_BIND_INIT_HANDLER(Cheat, Init);

GAMNET_BIND_TEST_HANDLER(
    TestSession, "Cheat_AddItem",
    Message::User::MsgCliSvr_Cheat_Req, Test_CheatItem_Req,
    Message::User::MsgSvrCli_Cheat_Ans, Test_CheatItem_Ans
);
