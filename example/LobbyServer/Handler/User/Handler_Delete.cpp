#include "Handler_Delete.h"
#include "../../../idl/MessageUser.h"

namespace Handler { namespace User {

Handler_Delete::Handler_Delete()
{
}

Handler_Delete::~Handler_Delete()
{
}

void Handler_Delete::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_Delete_Req req;
	MsgSvrCli_Delete_Ans ans;
	ans.error_code = ErrorCode::Success;

	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Delete_Req()");
		if(nullptr == session->GetComponent<UserData>())
		{
			throw GAMNET_EXCEPTION(ErrorCode::UndefineError);
		}

		Gamnet::Database::MySQL::ResultSet ret = Gamnet::Database::MySQL::Execute((int)DatabaseType::Account, 
			"CALL sp_account_delete('", session->account_id, "',", (int)session->account_type, ", 0)"
		);
		if (1 > ret.GetRowCount())
		{
			throw GAMNET_EXCEPTION(ErrorCode::UndefineError);
		}

		auto row = ret[0];
		int errorCode = row->getInt("Code");
		if (0 != errorCode)
		{
			switch (errorCode)
			{
			case 1329: // zero data
				throw GAMNET_EXCEPTION(ErrorCode::UndefineError, "error_code:", errorCode, ", message:", ret[0]->getString("Message"));
				break;
			default:
				throw GAMNET_EXCEPTION(ErrorCode::UndefineError, "error_code:", errorCode, ", message:", ret[0]->getString("Message"));
				break;
			}
		}
	
		session->RemoveComponent<UserData>();
	}
	catch (const Gamnet::Exception& e)
	{
		ans.error_code = (ErrorCode)e.error_code();
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	LOG(DEV, "MsgSvrCli_Delete_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_Delete_Req,
	Handler_Delete, Recv_Req,
	HandlerStatic
);

void Test_Delete_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Delete_Req req;
	Gamnet::Test::SendMsg(session, req);
}

void Test_Delete_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Delete_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_User_Delete",
	MsgCliSvr_Delete_Req, Test_Delete_Req,
	MsgSvrCli_Delete_Ans, Test_Delete_Ans
);

}}