#ifndef GAMNET_NETWORK_TCP_LINKMANAGER_H_
#define GAMNET_NETWORK_TCP_LINKMANAGER_H_

#include "Dispatcher.h"
#include "Packet.h"
#include "../LinkManager.h"
#include "../../Library/Json/json.h"

#include <memory>
#include <atomic>
#include <mutex>
#include <boost/bind.hpp>

namespace Gamnet { namespace Network { namespace Tcp {

template <class SESSION_T>
class LinkManager : public Network::LinkManager
{
protected :
	std::string _name;
public :
	Pool<SESSION_T, std::mutex, Session::Init> session_pool;
	SessionManager session_manager;

	LinkManager() : _name("TcpLinkManager"), session_pool()	{}
	virtual ~LinkManager()	{}

	void Listen(int port, int max_session, int keepAliveTime)
	{
		session_pool.Capacity(max_session);
		session_manager.Init(keepAliveTime);

		RegisterHandler(MsgHandler::MsgID_Connect_Req, &MsgHandler::Recv_Connect_Req, new Network::HandlerStatic<MsgHandler>());
		RegisterHandler(MsgHandler::MsgID_Reconnect_Req, &MsgHandler::Recv_Reconnect_Req, new Network::HandlerStatic<MsgHandler>());
		RegisterHandler(MsgHandler::MsgID_HeartBeat_Req, &MsgHandler::Recv_HeartBeat_Req, new Network::HandlerStatic<MsgHandler>());
		RegisterHandler(MsgHandler::MsgID_Close_Ntf, &MsgHandler::Recv_Close_Ntf, new Network::HandlerStatic<MsgHandler>());
		Network::LinkManager::Listen(port, max_session, keepAliveTime);
	}

	virtual void OnAccept(const std::shared_ptr<Link>& link)
	{
		//LOG(DEV, "[link_key:", link->link_key, "] accept link");
		const std::shared_ptr<SESSION_T> session = session_pool.Create();
		if(nullptr == session)
		{
			LOG(GAMNET_ERR, "create session instance fail(link_key:", link->link_key, ")");
			link->OnError(ErrorCode::CreateInstanceFailError);
			return;
		}
		session->session_key = ++Network::SessionManager::session_key;
		session->recv_packet = Packet::Create();	
		if (nullptr == session->recv_packet)
		{
			LOG(GAMNET_ERR, "can not create packet");
			link->OnError(ErrorCode::CreateInstanceFailError);
			return;
		}
		session->msg_seq = 0;
		link->AttachSession(session);
		session_manager.Add(session->session_key, session);;
	}

	virtual void OnClose(const std::shared_ptr<Link>& link, int reason)
	{
		//LOG(DEV, "[link_key:", link->link_key, "] close link");
		const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
		if(nullptr != session)
		{
			session->OnClose(reason);
			session->recv_packet = nullptr;
		}
		link->AttachSession(nullptr);
		Network::LinkManager::OnClose(link, reason);
	}

	virtual void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer)
	{
		std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
		if(NULL == session)
		{
			LOG(GAMNET_ERR, "invalid session(link_key:", link->link_key, ")");
			link->OnError(ErrorCode::InvalidSessionError);
			return;
		}

		session->expire_time = ::time(NULL);
		session->recv_packet->Append(buffer->ReadPtr(), buffer->Size());
		while(Packet::HEADER_SIZE <= (int)session->recv_packet->Size())
		{
			uint16_t totalLength = session->recv_packet->GetLength();
			if(Packet::HEADER_SIZE > totalLength )
			{
				LOG(GAMNET_ERR, "buffer underflow(read size:", totalLength, ")");
				link->OnError(ErrorCode::BufferUnderflowError);
				return;
			}

			if(totalLength >= session->recv_packet->Capacity())
			{
				LOG(GAMNET_ERR, "buffer overflow(read size:", totalLength, ")");
				link->OnError(ErrorCode::BufferOverflowError);
				return;
			}

			// if received bytes are not enough
			if(totalLength > (uint16_t)session->recv_packet->Size())
			{
				break;
			}

			uint32_t msg_seq = session->recv_packet->GetSEQ();
			if(msg_seq > session->msg_seq)
			{
				//LOG(DEV, "[link_key:", link->link_key, ", session_key:", session->session_key, "] receive message(msg_seq:", msg_seq, ", msg_id:", session->recv_packet->GetID(), ")");
				Singleton<Dispatcher<SESSION_T>>::GetInstance().OnRecvMsg(session, session->recv_packet);
				session = std::static_pointer_cast<SESSION_T>(link->session);
				if(nullptr == session)
				{
					link->OnError(ErrorCode::InvalidSessionError);
					return;
				}
				session->msg_seq = msg_seq;
			}
#ifdef _DEBUG
			else
			{
				LOG(WRN, "[link_key:", link->link_key, ", session_key:", session->session_key, "] discard message(msg_seq:", msg_seq, ", expect:", session->msg_seq+1, ")");
			}
#endif
			session->recv_packet->Remove(totalLength);

			if(0 < session->recv_packet->Size())
			{
				std::shared_ptr<Packet> packet = Packet::Create();
				if (NULL == packet)
				{
					LOG(ERR, "can not create buffer(link_key:", link->link_key, ")");
					link->OnError(ErrorCode::NullPacketError);
					return;
				}
				packet->Append(session->recv_packet->ReadPtr(), session->recv_packet->Size());
				session->recv_packet = packet;
			}
		}
	}

	std::shared_ptr<SESSION_T> FindSession(uint32_t session_key) {
		return std::static_pointer_cast<SESSION_T>(session_manager.Find(session_key));
	}
	
	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().RegisterHandler(msg_id, func, factory);
	}

	struct MsgHandler : public Network::IHandler
	{
		enum MSG_ID {
			MsgID_Connect_Req = 1,
			MsgID_Connect_Ans = 1,
			MsgID_Reconnect_Req = 2,
			MsgID_Reconnect_Ans = 2,
			MsgID_HeartBeat_Req = 3,
			MsgID_HeartBeat_Ans = 3,
			MsgID_Kickout_Ntf = 4,
			MsgID_Close_Ntf = 5
		};

		MsgHandler() {}
		virtual ~MsgHandler() {}

		void Recv_Connect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			//LOG(DEV, "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] receive connect request");
			session->session_token = Session::GenerateSessionToken(session->session_key);

			Json::Value ans;
			ans["error_code"] = 0;
			ans["session_key"] = session->session_key;
			ans["session_token"] = session->session_token;
			
			session->Send(MsgID_Connect_Ans, ans);
			//LOG(DEV, "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] send connect answer(session_key:", session->session_key, ", session_token:", session->session_token, ")");
			session->OnAccept();
		}
		void Recv_Reconnect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			std::shared_ptr<Link> link = session->link;

			Json::Value ans;
			ans["error_code"] = 0;
			try {
				Json::Value req;
				std::string json = std::string(packet->ReadPtr() + Packet::HEADER_SIZE, packet->Size());
				Json::Reader reader;
				if (false == reader.parse(json, req))
				{
					throw Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] parse error(msg:", json, ")");
				}

				uint32_t session_key = req["session_key"].asUInt();
				const std::string session_token = req["session_token"].asString();
				//LOG(DEV, "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] receive re-connect request");
				const std::shared_ptr<SESSION_T> other = Singleton<LinkManager<SESSION_T>>::GetInstance().FindSession(session_key);
				if (NULL == other)
				{
					throw Exception(GAMNET_ERRNO(ErrorCode::ConnectTimeoutError), "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] no cached data for session(session_key:", session_key, ")");
				}

				if(session_token != other->session_token)
				{
					throw Exception(GAMNET_ERRNO(ErrorCode::InvalidSessionTokenError), "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] invald session token(expect:", other->session_token, ", receive:", session_token, ")");
				}
				
				std::shared_ptr<Link> _link = other->link;
				if (NULL != _link)
				{
					_link->AttachSession(NULL);
				}

				other->recv_packet = session->recv_packet;
				other->session_token = Session::GenerateSessionToken(other->session_key);
				link->AttachSession(other);
				link->session->OnAccept();

				Singleton<LinkManager<SESSION_T>>::GetInstance().session_manager.Remove(session->session_key);
				ans["session_key"] = other->session_key;
				ans["session_token"] = other->session_token;
				ans["msg_seq"] = (uint32_t)(other->msg_seq);
			}
			catch (const Exception& e)
			{
				//link->AttachSession(NULL);
				ans["error_code"] = e.error_code();
				LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			}

			//LOG(DEV, "[link_key:", link->link_key, ", session_key:", NULL == link->session ? 0 : link->session->session_key, "] send re-connect answer(error_code:", ans["error_code"].asInt(), ", session_token:", NULL == link->session ? "<null>" : link->session->session_token, ")");

			Json::StyledWriter writer;
			std::string str = writer.write(ans);

			Packet::Header header;
			header.msg_id = MsgID_Reconnect_Ans;
			header.msg_seq = session->msg_seq;
			header.length = (uint16_t)(Packet::HEADER_SIZE + str.length()+1);

			std::shared_ptr<Packet> ans_packet = Packet::Create();
			if(NULL == ans_packet)
			{
				return;
			}
			ans_packet->Write(header, str.c_str(), str.length()+1);
			link->AsyncSend(ans_packet);
		}
		void Recv_HeartBeat_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			//LOG(DEV, "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] receive heartbeat request(msg_seq:", packet->GetSEQ(), ", expect:", session->msg_seq+1, ")");
			Json::Value ans;
			ans["error_code"] = 0;
			ans["msg_seq"] = (uint32_t)session->msg_seq;
			session->Send(MsgID_HeartBeat_Ans, ans);
			//LOG(DEV, "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] send heartbeat ans(msg_seq:", session->msg_seq, ")");
		}
		void Recv_Close_Ntf(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			std::shared_ptr<Link> link = session->link;
			if(nullptr != link)
			{
				link->OnError(ErrorCode::Success);
			}
			Singleton<LinkManager<SESSION_T>>::GetInstance().session_manager.Remove(session->session_key);
		}
	};

	Json::Value State()
	{
		Json::Value root = Network::LinkManager::State();
		root["name"] = _name;

		Json::Value session;
		session["capacity"] = (unsigned int)session_pool.Capacity();
		session["available"] = (unsigned int)session_pool.Available();
		session["running_count"] = (unsigned int)session_manager.Size();
		root["session"] = session;

#ifdef _DEBUG
		Json::Value message;
		for (auto itr : Singleton<Dispatcher<SESSION_T>>::GetInstance().mapHandlerCallStatistics_)
		{
			Json::Value statistics;
			statistics["msg_id"] = (unsigned int)itr.second->msg_id;
			statistics["begin_count"] = (unsigned int)itr.second->begin_count;
			statistics["finish_count"] = (unsigned int)itr.second->finish_count;
			statistics["elapsed_time"] = (unsigned int)itr.second->elapsed_time;
			if (0 == itr.second->elapsed_time || 0 == itr.second->finish_count)
			{
				statistics["average_time"] = 0;
			}
			else
			{
				statistics["average_time"] = (int)(itr.second->elapsed_time / itr.second->finish_count);
			}
			message.append(statistics);
		}
		root["message"] = message;
#endif
		return root;
	}
};

}}}
#endif /* LISTENER_H_ */
