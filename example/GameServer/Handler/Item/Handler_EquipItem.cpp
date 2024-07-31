#include "Handler_EquipItem.h"
#include "../../Component/UserData.h"
#include "../../Component/UserData/Bag.h"
#include "../../Component/UserData/Item.h"
#include "../../Component/UserData/Suit.h"
#include "../../../idl/MessageUser.h"

namespace Handler {	namespace Item {

void Handler_EquipItem::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_EquipItem_Req& req)
{
	Message::Item::MsgSvrCli_EquipItem_Ans ans;
	ans.error_code = Message::ErrorCode::Success;

	try {
		LOG(DEV, "Message::Item::MsgCliSvr_EquipItem_Req()");
		auto pUserData = session->GetComponent<Component::UserData>();
		if(nullptr == pUserData)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}
		
		Transaction transaction(session);
		if (false == transaction(::Item::Equip(session, req.item_no)))
		{
			transaction.Rollback();
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		transaction.Commit();
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Message::Item::MsgSvrCli_EquipItem_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::Item::MsgCliSvr_EquipItem_Req,
	Handler_EquipItem, Recv_Req,
	HandlerStatic
);

void Test_EquipItem_Req(const std::shared_ptr<TestSession>& session)
{
	std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
	if (nullptr == pUserData)
    {
        throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
    }

	auto pBag = pUserData->pBag;

	int64_t itemNo = 0;
	std::list<Message::ItemData> items;
	pBag->Serialize(items);;
	   
	for (auto item : items)
	{
        auto meta = Gamnet::Singleton<::Item::Manager>::GetInstance().FindMeta(item.item_index);
        if (Message::ItemType::Equip == meta->Type)
        {
            itemNo = item.item_no;
            break;
        }
	}

	Message::Item::MsgCliSvr_EquipItem_Req req;
	req.item_no = itemNo;
	Gamnet::Test::SendMsg(session, req);
}

void Test_EquipItem_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_EquipItem_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Item_EquipItem",
	Message::Item::MsgCliSvr_EquipItem_Req, Test_EquipItem_Req,
	Message::Item::MsgSvrCli_EquipItem_Ans, Test_EquipItem_Ans
);

void Test_EquipItem_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_EquipItem_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}

		std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
		if (nullptr == pUserData)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		auto pBag = pUserData->pBag;
		auto pSuit = pUserData->pSuit;

		auto pItem = pBag->Find(ntf.item_no);
		pBag->Remove(pItem->item_no, pItem->count);
		pSuit->Equip(pItem);
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::Item::MsgSvrCli_EquipItem_Ntf, Test_EquipItem_Ntf
);

}}