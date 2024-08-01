#include "Handler_OpenPackage.h"
#include "../../Component/UserData.h"
#include "../../Component/UserData/Bag.h"
#include "../../Component/UserData/Item.h"

namespace Handler { namespace Item {

void Handler_OpenPackage::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_OpenPackage_Req& req)
{
	Message::Item::MsgSvrCli_OpenPackage_Ans ans;
	ans.error_code = Message::ErrorCode::Success;
	
	try {
		LOG(DEV, "Message::Item::MsgCliSvr_OpenPackage_Req()");
		auto pUserData = session->pUserData;
		if(0 == pUserData->user_no)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		auto pBag = pUserData->pBag;
		auto pItem = pBag->Find(req.item_no);
		if (nullptr == pItem)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		if (nullptr == pItem->package)
        {
            throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
        }

		Transaction transaction(session);
		
		if (false == transaction(::Item::RemoveFromBag(session, req.item_no, 1)))
		{
			transaction.Rollback();
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}
		
		auto pItemMeta = pItem->meta;
		for (const auto package : pItemMeta->Package)
        {
			std::shared_ptr<::Item::Data> data = ::Item::Create(package->Code, package->Count);
			if (nullptr == data)
			{
				continue;
			}

            if (false == transaction(::Item::InsertIntoBag(session, data)))
            {
                transaction.Rollback();
                throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
            }
        }
		transaction.Commit();
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Message::Item::MsgSvrCli_OpenPackage_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::Item::MsgCliSvr_OpenPackage_Req,
	Handler_OpenPackage, Recv_Req,
	HandlerStatic
);

void Test_OpenPackage_Req(const std::shared_ptr<TestSession>& session)
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
		if (Message::ItemType::Package == meta->Type)
		{
			itemNo = item.item_no;
			break;
		}
	}

	Message::Item::MsgCliSvr_OpenPackage_Req req;
	req.item_no = itemNo;
	Gamnet::Test::SendMsg(session, req);
}

void Test_OpenPackage_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_OpenPackage_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_OpenPackageLobby_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Item_OpenPackage",
	Message::Item::MsgCliSvr_OpenPackage_Req, Test_OpenPackage_Req,
	Message::Item::MsgSvrCli_OpenPackage_Ans, Test_OpenPackage_Ans
);

}}