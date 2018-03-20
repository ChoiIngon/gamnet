#ifndef GAMNET_NETWORK_TCP_LINKMANAGER_H_
#define GAMNET_NETWORK_TCP_LINKMANAGER_H_

#include "Dispatcher.h"
#include "Packet.h"
#include "Link.h"
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
public :
	Pool<SESSION_T, std::mutex, Session::Init> session_pool;
	Pool<Link, std::mutex, Link::Init> link_pool;
	SessionManager session_manager;

	LinkManager() : session_pool()	
	{
		_name = "Gamnet::Network::Tcp::LinkManager";
	}
	virtual ~LinkManager()	{}

	void Listen(int port, int max_session, int keepAliveTime)
	{
		link_pool.Capacity(max_session * 3);

		session_pool.Capacity(max_session);
		session_manager.Init(keepAliveTime);

		RegisterHandler(MsgHandler::MsgID_Connect_Req, &MsgHandler::Recv_Connect_Req, new Network::HandlerStatic<MsgHandler>());
		RegisterHandler(MsgHandler::MsgID_Reconnect_Req, &MsgHandler::Recv_Reconnect_Req, new Network::HandlerStatic<MsgHandler>());
		RegisterHandler(MsgHandler::MsgID_HeartBeat_Req, &MsgHandler::Recv_HeartBeat_Req, new Network::HandlerStatic<MsgHandler>());
		RegisterHandler(MsgHandler::MsgID_Close_Ntf, &MsgHandler::Recv_Close_Ntf, new Network::HandlerStatic<MsgHandler>());
		Network::LinkManager::Listen(port, max_session, keepAliveTime);
	}

	virtual std::shared_ptr<Network::Link> Create() override
	{
		std::shared_ptr<Link> link = link_pool.Create();
		if(nullptr == link)
		{
			LOG(GAMNET_ERR, "can not create 'Tcp::Link' instance");
			return nullptr;
		}

		link->link_key = ++LinkManager::link_key;

		if(nullptr == link->read_buffer)
		{
			LOG(GAMNET_ERR, "can not create read buffer");
			return nullptr;
		}
		
		if(nullptr == link->recv_packet)
		{
			LOG(GAMNET_ERR, "can not create recv packet");
			return nullptr;
		}
		
		return link;
	}
	virtual size_t Available()
	{
		return link_pool.Available();
	}
	
	virtual size_t Capacity() const
	{
		return link_pool.Capacity();
	}
	virtual void Capacity(size_t count)
	{
		link_pool.Capacity(count);
	}

	virtual void OnAccept(const std::shared_ptr<Network::Link>& link) override
	{
		const std::shared_ptr<SESSION_T> session = session_pool.Create();
		if(nullptr == session)
		{
			LOG(GAMNET_ERR, "create session instance fail(link_key:", link->link_key, ")");
			link->OnError(ErrorCode::CreateInstanceFailError);
			return;
		}
		
		session->msg_seq = 0;
		link->AttachSession(session);
		session_manager.Add(session->session_key, session);
	}

	virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason) override
	{
		if(nullptr != link->session)
		{
			link->session->OnClose(reason);
		}
		link->AttachSession(nullptr);
		Network::LinkManager::OnClose(link, reason);
	}

	virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) override
	{
		std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
		if(NULL == session)
		{
			return;
		}

		const std::shared_ptr<Packet>& packet = std::static_pointer_cast<Packet>(buffer);
		
		uint32_t msg_seq = packet->GetSEQ();
		if(msg_seq > session->msg_seq)
		{
			session->msg_seq = msg_seq;
			Singleton<Dispatcher<SESSION_T>>::GetInstance().OnRecvMsg(session, packet);
		}
#ifdef _DEBUG
		else
		{
			LOG(WRN, "[link_key:", link->link_key, ", session_key:", session->session_key, "] discard message(msg_seq:", msg_seq, ", expect:", session->msg_seq + 1, ")");
		}
#endif
	}

	std::shared_ptr<SESSION_T> FindSession(uint32_t session_key) {
		return std::static_pointer_cast<SESSION_T>(session_manager.Find(session_key));
	}
	
	void DestroySession(uint32_t session_key)
	{
		std::shared_ptr<Gamnet::Network::Session> session = session_manager.Find(session_key);
		if(nullptr == session)
		{
			LOG(WRN, "can not find session(session_key:", session_key, ")");
			return;
		}
		std::shared_ptr<Network::Link> link = session->link;
		if(nullptr != link)
		{
			link->AttachSession(nullptr);
		}

		session_manager.Remove(session->session_key);
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
			
			Json::StyledWriter writer;
			std::string str = writer.write(ans);

			Packet::Header header;
			header.msg_id = MsgID_Connect_Ans;
			header.msg_seq = session->msg_seq;
			header.length = (uint16_t)(Packet::HEADER_SIZE + str.length()+1);

			std::shared_ptr<Packet> ans_packet = Packet::Create();
			if(NULL == ans_packet)
			{
				return;
			}
			ans_packet->Write(header, str.c_str(), str.length()+1);
			session->AsyncSend(ans_packet);
			//LOG(DEV, "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] send connect answer(session_key:", session->session_key, ", session_token:", session->session_token, ")");
			session->OnAccept();
		}
		void Recv_Reconnect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			std::shared_ptr<Network::Tcp::Link> link = std::static_pointer_cast<Network::Tcp::Link>(session->link);
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
				
				const std::shared_ptr<SESSION_T> other = Singleton<LinkManager<SESSION_T>>::GetInstance().FindSession(session_key);
				if (NULL == other)
				{
					throw Exception(GAMNET_ERRNO(ErrorCode::InvalidSessionKeyError), "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] can not find session data for reconnect(session_key:", session_key, ")");
				}

				if(session_token != other->session_token)
				{
					throw Exception(GAMNET_ERRNO(ErrorCode::InvalidSessionTokenError), "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] invalid session token(expect:", other->session_token, ", receive:", session_token, ")");
				}
				
				std::shared_ptr<Network::Tcp::Link> _link = std::static_pointer_cast<Network::Tcp::Link>(other->link);
				if (NULL != _link)
				{
					link->recv_packet = _link->recv_packet;
					_link->AttachSession(NULL);
				}

				link->AttachSession(other);
//				other->recv_packet = session->recv_packet;
				other->OnAccept();
				
				Singleton<LinkManager<SESSION_T>>::GetInstance().session_manager.Remove(session->session_key);
			}
			catch (const Exception& e)
			{
				link->AttachSession(nullptr);
				ans["error_code"] = e.error_code();
				LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			}

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
			session->AsyncSend(ans_packet);
		}
		void Recv_HeartBeat_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			Json::Value ans;
			ans["error_code"] = 0;
			ans["msg_seq"] = (uint32_t)session->msg_seq;
			Json::StyledWriter writer;
			std::string str = writer.write(ans);

			Packet::Header header;
			header.msg_id = MsgID_HeartBeat_Ans;
			header.msg_seq = session->msg_seq;
			header.length = (uint16_t)(Packet::HEADER_SIZE + str.length()+1);

			std::shared_ptr<Packet> ans_packet = Packet::Create();
			if(NULL == ans_packet)
			{
				return;
			}
			ans_packet->Write(header, str.c_str(), str.length()+1);
			session->AsyncSend(ans_packet);
		}
		void Recv_Close_Ntf(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			std::shared_ptr<Network::Link> link = session->link;
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
