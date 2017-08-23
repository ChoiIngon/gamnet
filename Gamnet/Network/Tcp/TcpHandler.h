#ifndef GAMNET_NETWORK_TCP_MSGHANDLER_H_
#define GAMNET_NETWORK_TCP_MSGHANDLER_H_

#include "Session.h"
#include "../Handler.h"
#include "../../Library/Json/json.h"
#include "../../Library/MD5.h"
#include "../../Library/Random.h"
#include "../../Library/Singleton.h"

namespace Gamnet {	namespace Network {	namespace Tcp {
	template <class SESSION_T>
	struct TcpHandler : public Network::IHandler
	{
		TcpHandler() {};
		virtual ~TcpHandler() {};
		
		const int Recv_Connect_Req_MsgID = 1;
		void Recv_Connect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			Json::Value root;
			root["error_code"] = 0;
			root["link_key"] = session->link.lock()->link_key;
			root["session_key"] = session->session_key;
			Json::StyledWriter writer;
			std::string json = writer.write(root);

			std::shared_ptr<Packet> ans = Packet::Create();
			ans->Write(Recv_Connect_Req_MsgID, session->msg_seq, json.c_str(), json.length());
			session->AsyncSend(ans);

			session->OnAccept();
		}

		const int Recv_Reconnect_Req_MsgID = 2;
		void Recv_Reconnect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			Json::Value root;
			root["error_code"] = 0;
			try {
				std::string json = std::string(packet->ReadPtr() + Packet::HEADER_SIZE, packet->Size());
				Json::Reader reader;
				Json::Value req;

				if(false == reader.parse(json, req))
				{
					throw Exception(ErrorCode::MessageFormatError, "parse error");
				}

				uint64_t session_key = req["session_key"].asInt64();
				std::string session_token = req["session_token"].asString();

				const std::shared_ptr<Session> prev_session = Singleton<LinkManager<SESSION_T>>::GetInstance().session_manager.FindSession(session_key);
				if(NULL == prev_session)
				{
					throw Exception(1, "no cache data");
				}

				if(session_token != prev_session->session_token)
				{
					throw Exception(2, "invalid session token");
				}

				ntf.error_code = DuplicateConnectionError;
				LOG(DEV, "MsgSvrCli_Kickout_Ntf(session_key:", other->sessionKey_, ")");
				Gamnet::Network::SendMsg(other, ntf);

				prev_session->socket_->cancel();
				session->socket_->cancel();
				prev_session->socket_ = session->socket_;
				prev_session->AsyncRead();
			}
			catch (const Exception& e)
			{
				root["error_code"] = e.error_code();
				LOG(GAMNET_ERR, e.what());
			}

			Json::StyledWriter writer;
			std::string json = writer.write(root);

			std::shared_ptr<Packet> ans = Packet::Create();
			ans->Write(Recv_Reconnect_Req_MsgID, session->msg_seq, json.c_str(), json.length());
			session->AsyncSend(ans);
			session->OnAccept();
		}
	};
}}}
#endif /* ROUTER_MSGHANDLER_H_ */
