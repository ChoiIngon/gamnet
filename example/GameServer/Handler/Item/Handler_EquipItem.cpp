#include "Handler_EquipItem.h"
#include "../../Component/UserData.h"
#include "../../Component/Bag.h"
#include "../../Component/Item.h"
#include "../../Component/Suit.h"

namespace Handler {	namespace Item {

	Handler_EquipItem::Handler_EquipItem()
	{
	}

	Handler_EquipItem::~Handler_EquipItem()
	{
	}

	void Handler_EquipItem::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_EquipItem_Req& req)
	{
		Message::Item::MsgSvrCli_EquipItem_Ans ans;
		ans.error_code = Message::ErrorCode::Success;

		try {
			LOG(DEV, "Message::Item::MsgCliSvr_EquipItem_Req()");
			if (nullptr == session->GetComponent<Component::UserData>())
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
			}

			auto bag = session->GetComponent<Component::Bag>();
			auto item = bag->Find(req.item_seq);
			if (nullptr == item)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}
			if (nullptr == item->equip)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}
			auto suit = session->GetComponent<Component::Suit>();
			session->StartTransaction();
			suit->Equip(item);
			session->Commit();
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
		Message::Item::MsgCliSvr_EquipItem_Req req;
		req.item_seq = 0;
		for (auto& itr : session->items)
		{
			auto& item = itr.second;
			auto meta = Gamnet::Singleton<::Item::Manager>::GetInstance().FindMeta(item.item_index);
			if (Message::ItemType::Equip == meta->type)
			{
				req.item_seq = item.item_seq;
				break;
			}
		}
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
			//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_EquipItemLobby_Ans");
		}
		catch (const Gamnet::Exception& e) {
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}

		/*
		if (0 == ans.item_data.item_count)
		{
			session->items.erase(ans.item_data.item_seq);
		}
		else
		{
			session->items[ans.item_data.item_seq] = ans.item_data;
		}
		*/
		session->Next();
	}

	GAMNET_BIND_TEST_HANDLER(
		TestSession, "Test_Item_EquipItem",
		Message::Item::MsgCliSvr_EquipItem_Req, Test_EquipItem_Req,
		Message::Item::MsgSvrCli_EquipItem_Ans, Test_EquipItem_Ans
	);

}}