#ifndef GAMNET_NETWORK_TCP_LINKMANAGER_H_
#define GAMNET_NETWORK_TCP_LINKMANAGER_H_

#include "Dispatcher.h"
#include "Packet.h"
#include "../LinkManager.h"
#include "../../Library/Json/json.h"
#include "../../Library/MD5.h"
#include "../../Library/Random.h"
#include "../../Library/Singleton.h"

#include "MsgNetwork.h"

#include <memory>
#include <atomic>
#include <mutex>
#include <boost/bind.hpp>


namespace Gamnet { namespace Network { namespace Tcp {

template <class SESSION_T>
class LinkManager : public Network::LinkManager
{
	class Network_ConnectHandler : public IHandler
	{
	public :
		void Recv_Connect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			Json::Value root;
			root["error_code"] = 0;
			root["link_key"] = session->link.lock()->link_key;
			root["session_key"] = session->session_key;
			Json::StyledWriter writer;
			std::string json = writer.write(root);

			std::shared_ptr<Packet> ans = Packet::Create();
			ans->Write(0001, session->msg_seq, json.c_str(), json.length());
			session->AsyncSend(ans);
		}
		void Recv_Reconnect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			Json::Value root;
			root["error_code"] = 0;
			try {
				/*
				std::string json = std::string(packet->ReadPtr() + Packet::HEADER_SIZE, packet->Size());
				Json::Reader reader;
				Json::Value req;
				if(false == reader.parse(json, req))
				{
					throw Exception(1, "parse error");
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
				*/
			}
			catch(const Exception& e)
			{
				root["error_code"] = e.error_code();
				LOG(GAMNET_ERR, e.what());
			}

			Json::StyledWriter writer;
			std::string json = writer.write(root);

			std::shared_ptr<Packet> ans = Packet::Create();
			ans->Write(0002, session->msg_seq, json.c_str(), json.length());
			session->AsyncSend(ans);
		}
	};
public :
	Pool<SESSION_T, std::mutex, Session::Init> session_pool;
	SessionManager session_manager;

	LinkManager() : session_pool()	{}
	virtual ~LinkManager()	{}

	void Listen(int port, int max_session, int keepAliveTime)
	{
		RegisterHandler(MsgNetwork_Connect_Req::MSG_ID, &Network_ConnectHandler::Recv_Connect_Req,	new HandlerStatic<Network_ConnectHandler>());
		RegisterHandler(MsgNetwork_Reconnect_Req::MSG_ID, &Network_ConnectHandler::Recv_Reconnect_Req,	new HandlerStatic<Network_ConnectHandler>());

		session_pool.Capacity(max_session);
		session_manager.Init(keepAliveTime);
		Network::LinkManager::Listen(port, max_session, keepAliveTime);
	}

	virtual void OnAccept(const std::shared_ptr<Link>& link)
	{
		Network::LinkManager::OnAccept(link);
		const std::shared_ptr<SESSION_T> session = session_pool.Create();
		if(NULL == session)
		{
			link->OnError(EINVAL);
			return;
		}
		session_manager.Add(session->session_key, session);
		link->AttachSession(session);
	}

	virtual void OnClose(const std::shared_ptr<Link>& link, int reason)
	{
		const std::shared_ptr<Network::Session>& session = link->session;
		if(NULL != session)
		{
			link->AttachSession(std::shared_ptr<Network::Session>(NULL));
		}
		Network::LinkManager::OnClose(link, reason);
	}

	virtual void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer)
	{
		const std::shared_ptr<SESSION_T>& session = std::static_pointer_cast<SESSION_T>(link->session);
		if(NULL == session)
		{
			LOG(GAMNET_ERR, "invalid session(link_key:", link->link_key, ")");
			link->OnError(EINVAL);
			return;
		}

		session->recv_packet->Append(buffer->ReadPtr(), buffer->Size());
		while(Packet::HEADER_SIZE <= (int)session->recv_packet->Size())
		{
			uint16_t totalLength = session->recv_packet->GetLength();
			if(Packet::HEADER_SIZE > totalLength )
			{
				LOG(GAMNET_ERR, "buffer underflow(read size:", totalLength, ")");
				link->OnError(EOVERFLOW);
				return;
			}

			if(totalLength >= session->recv_packet->Capacity())
			{
				LOG(GAMNET_ERR, "buffer overflow(read size:", totalLength, ")");
				link->OnError(EOVERFLOW);
				return;
			}

			// 데이터가 부족한 경우
			if(totalLength > (uint16_t)session->recv_packet->Size())
			{
				break;
			}

			Singleton<Dispatcher<SESSION_T>>::GetInstance().OnRecvMsg(session, session->recv_packet);
			session->recv_packet->Remove(totalLength);

			std::shared_ptr<Packet> packet = Packet::Create();
			if(NULL == packet)
			{
				LOG(GAMNET_ERR, "Can't create more buffer(link_key:", link->link_key, ")");
				link->OnError(EOVERFLOW);
				return;
			}

			if(0 < session->recv_packet->Size())
			{
				packet->Append(session->recv_packet->ReadPtr(), session->recv_packet->Size());
			}

			session->recv_packet = packet;
		}
	}

	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().RegisterHandler(msg_id, func, factory);
	}
};

}}}
#endif /* LISTENER_H_ */
