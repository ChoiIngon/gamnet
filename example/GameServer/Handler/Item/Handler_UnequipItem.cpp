#include "Handler_UnequipItem.h"
#include "../../Component/UserData.h"

namespace Handler {	namespace Item {

void Handler_UnequipItem::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_UnequipItem_Req& req)
{
	Message::Item::MsgSvrCli_UnequipItem_Ans ans;
	ans.error_code = Message::ErrorCode::Success;

	try {
		LOG(DEV, "Message::Item::MsgCliSvr_UnequipItem_Req()");
		Transaction transaction(session);
		if(false == transaction(::Item::Unequip(session, req.part_type)))
        {
            transaction.Rollback();
            throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
        }
		transaction.Commit();
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Message::Item::MsgSvrCli_UnequipItem_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::Item::MsgCliSvr_UnequipItem_Req,
	Handler_UnequipItem, Recv_Req,
	HandlerStatic
);

void Test_UnequipItem_Req(const std::shared_ptr<TestSession>& session)
{
	std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
	if (nullptr == pUserData)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
	}

	auto pSuit = pUserData->pSuit;
	Message::EquipItemPartType partType = Message::EquipItemPartType::Invalid;
	for (int i= 0; i < (int)Message::EquipItemPartType::Max; i++)
	{
		auto pItem = pSuit->Find((Message::EquipItemPartType)i);
		if(nullptr != pItem)
        {
			partType = pItem->meta->Equip->Part;
			break;
        }
	}
	

	Message::Item::MsgCliSvr_UnequipItem_Req req;
	req.part_type = partType;
	Gamnet::Test::SendMsg(session, req);
}

void Test_UnequipItem_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_UnequipItem_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_UnequipItemLobby_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Item_UnequipItem",
	Message::Item::MsgCliSvr_UnequipItem_Req, Test_UnequipItem_Req,
	Message::Item::MsgSvrCli_UnequipItem_Ans, Test_UnequipItem_Ans
);

}}