#include "Handler_OpenPackage.h"
#include "../../Component/UserData.h"
#include "../../Component/Bag.h"
#include "../../Component/Item.h"

namespace Handler { namespace Item {

Handler_OpenPackage::Handler_OpenPackage()
{
}

Handler_OpenPackage::~Handler_OpenPackage()
{
}

void Handler_OpenPackage::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_OpenPackage_Req& req)
{
	Message::Item::MsgSvrCli_OpenPackage_Ans ans;
	ans.error_code = Message::ErrorCode::Success;
	
	try {
		LOG(DEV, "Lobby::MsgCliSvr_OpenPackage_Req()");
		if(nullptr == session->GetComponent<Component::UserData>())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		session->StartTransaction();
		auto bag = session->GetComponent<Component::Bag>();
		auto item = bag->Find(req.item_seq);
		if(nullptr == item)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		if(nullptr == item->package)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		item->package->Use();
		session->Commit();
		ans.item_data = *item;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Item::MsgSvrCli_OpenPackage_Ans(error_code:", (int)ans.error_code, ")");
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
	Message::Item::MsgCliSvr_OpenPackage_Req req;
	req.item_seq = 0;
	for(auto& itr : session->items)
	{
		auto& item = itr.second;
		if(Message::ItemType::Package == item.item_type)
		{
			req.item_seq = item.item_seq;
			break;
		}
	}
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

	if(0 == ans.item_data.item_count)
	{
		session->items.erase(ans.item_data.item_seq);
	}
	else
	{
		session->items[ans.item_data.item_seq] = ans.item_data;
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Item_OpenPackage",
	Message::Item::MsgCliSvr_OpenPackage_Req, Test_OpenPackage_Req,
	Message::Item::MsgSvrCli_OpenPackage_Ans, Test_OpenPackage_Ans
);

}}