#include "Handler_SellItem.h"
#include "../../Component/UserData.h"

namespace Handler {	namespace Item {

	Handler_SellItem::Handler_SellItem()
	{
	}

	Handler_SellItem::~Handler_SellItem()
	{
	}

	void Handler_SellItem::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_SellItem_Req& req)
	{
		Message::Item::MsgSvrCli_SellItem_Ans ans;
		ans.error_code = Message::ErrorCode::Success;

		try {
			LOG(DEV, "Message::Item::MsgCliSvr_SellItem_Req()");
			if (nullptr == session->GetComponent<Component::UserData>())
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
			}

			auto bag = session->GetComponent<Component::Bag>();
			/*
			auto item = bag->Find(req.item_seq);
			if (nullptr == item)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}
			if (nullptr == item->meta->price)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}

			session->StartTransaction();
			if(nullptr != item->equip)
			{
				if(Message::EquipItemPartType::Invalid != item->equip->part)
				{
					std::shared_ptr<Component::Suit> suit = session->GetComponent<Component::Suit>();
					suit->Unequip(item->equip->part);
				}
			}
			bag->Remove(req.item_seq, req.item_count);

			std::shared_ptr<Component::UserCounter> counter = session->GetComponent<Component::UserCounter>();
			//counter->ChangeCount(item->meta->price->type, item->meta->price->value * req.item_count);
			//session->Commit();
			*/
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
			ans.error_code = (Message::ErrorCode)e.error_code();
		}
		LOG(DEV, "Message::Item::MsgSvrCli_SellItem_Ans(error_code:", (int)ans.error_code, ")");
		Gamnet::Network::Tcp::SendMsg(session, ans);
	}

	GAMNET_BIND_TCP_HANDLER(
		UserSession,
		Message::Item::MsgCliSvr_SellItem_Req,
		Handler_SellItem, Recv_Req,
		HandlerStatic
	);

	void Test_SellItem_Req(const std::shared_ptr<TestSession>& session)
	{
		Message::Item::MsgCliSvr_SellItem_Req req;
		req.item_no = 0;
		/*
		for (auto& itr : session->items)
		{
			auto& item = itr.second;
			auto meta = Gamnet::Singleton<::Item::Manager>::GetInstance().FindMeta(item.item_index);
			if (Message::ItemType::Equip == meta->Type)
			{
				req.item_no = item.item_no;
				break;
			}
		}
		*/
		Gamnet::Test::SendMsg(session, req);
	}

	void Test_SellItem_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
	{
		Message::Item::MsgSvrCli_SellItem_Ans ans;
		try {
			if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
			}
			//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_SellItemLobby_Ans");
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
		TestSession, "Test_Item_SellItem",
		Message::Item::MsgCliSvr_SellItem_Req, Test_SellItem_Req,
		Message::Item::MsgSvrCli_SellItem_Ans, Test_SellItem_Ans
	);

}}