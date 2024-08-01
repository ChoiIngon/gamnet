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
			auto pUserData = session->pUserData;
			if (0 == pUserData->user_no)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
			}
			auto pBag = pUserData->pBag;

			auto pItem = pBag->Find(req.item_no);
			if (nullptr == pItem)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}
			
			if (nullptr == pItem->meta->Price)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}

			Transaction transaction(session);
			if (false == transaction(::Item::RemoveFromBag(session, req.item_no, req.item_count)))
			{
				transaction.Rollback();
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}

			if(false == transaction(Component::Counter::UpdateValue(session, pItem->meta->Price->Type, pItem->meta->Price->Value * req.item_count)))
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
		std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
		if (nullptr == pUserData)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		auto pBag = pUserData->pBag;

		int64_t itemNo = 0;
		std::list<Message::ItemData> items;
		pBag->Serialize(items);

		for (auto item : items)
		{
			auto meta = Gamnet::Singleton<::Item::Manager>::GetInstance().FindMeta(item.item_index);
			if (Message::ItemType::Equip == meta->Type)
			{
				itemNo = item.item_no;
				break;
			}
		}

		Message::Item::MsgCliSvr_SellItem_Req req;
		req.item_no = itemNo;
		req.item_count = 1;
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