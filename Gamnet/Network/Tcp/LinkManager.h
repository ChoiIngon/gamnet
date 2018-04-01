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
	enum MSG_ID {
		MsgID_CliSvr_Connect_Req = 1,
		MsgID_SvrCli_Connect_Ans = 1,
		MsgID_CliSvr_Reconnect_Req = 2,
		MsgID_SvrCli_Reconnect_Ans = 2,
		MsgID_CliSvr_HeartBeat_Req = 3,
		MsgID_SvrCli_HeartBeat_Ans = 3,
		MsgID_SvrCli_Kickout_Ntf = 4,
		MsgID_CliSvr_Close_Ntf = 5,
		MsgID_Max = 6
	};
private :
	struct LinkFactory {
		LinkManager* const link_manager;
		LinkFactory(LinkManager* linkManager) : link_manager(linkManager) {}

		Link* operator() ()
		{
			return new Link(link_manager);
		}
	};

	std::function<void(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer)> handlers[MSG_ID::MsgID_Max];

	std::mutex lock;
	std::map<uint32_t, std::shared_ptr<Network::Link>> links;
public :
	SessionManager session_manager;
	Pool<SESSION_T, std::mutex, Network::Session::InitFunctor> session_pool;
	Pool<Link, std::mutex, Network::Link::InitFunctor> link_pool;

	LinkManager() : session_pool(), link_pool(65535, LinkFactory(this))
	{
		_name = "Gamnet::Network::Tcp::LinkManager";
	}

	virtual ~LinkManager()	
	{
	}

	void Listen(int port, int max_session, int keepAliveTime)
	{
		session_pool.Capacity(max_session);
		session_manager.Init(keepAliveTime);

		link_pool.Capacity(max_session);

		handlers[0] = nullptr;
		handlers[MsgID_CliSvr_Connect_Req] = std::bind(&LinkManager::Recv_Connect_Req, this, std::placeholders::_1, std::placeholders::_2);
		handlers[MsgID_CliSvr_Reconnect_Req] = std::bind(&LinkManager::Recv_Reconnect_Req, this, std::placeholders::_1, std::placeholders::_2);
		handlers[MsgID_CliSvr_HeartBeat_Req] = std::bind(&LinkManager::Recv_HeartBeat_Req, this, std::placeholders::_1, std::placeholders::_2);
		handlers[MsgID_CliSvr_Close_Ntf] = std::bind(&LinkManager::Recv_Close_Ntf, this, std::placeholders::_1, std::placeholders::_2);

		Network::LinkManager::Listen(port);
	}

	virtual std::shared_ptr<Network::Link> Create() override
	{
		std::shared_ptr<Link> link = link_pool.Create();
		if(nullptr == link)
		{
			LOG(GAMNET_ERR, "[link_manager:", _name, "] can not create 'Tcp::Link' instance");
			return nullptr;
		}
		return link;
	}

	virtual size_t Available() override
	{
		return link_pool.Available();
	}
	
	virtual size_t Capacity() override
	{
		return link_pool.Capacity();
	}
	
	virtual size_t Size() override
	{
		return session_manager.Size();
	}

	virtual void OnAccept(const std::shared_ptr<Network::Link>& link) override
	{
	}

	virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason) override
	{
		std::shared_ptr<Network::Session> session = link->session;
		if(nullptr != session)
		{
			session->OnClose(reason);
		}

		link->AttachSession(nullptr);
		std::lock_guard<std::mutex> lo(lock);
		links.erase(link->link_key);
	}

	virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) override
	{
		const std::shared_ptr<Packet>& packet = std::static_pointer_cast<Packet>(buffer);
		uint32_t msgID = packet->GetID();
		if(MSG_ID::MsgID_Max <= msgID)
		{
			std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
			if(nullptr == session)
			{
				LOG(ERR, "[link_key:", link->link_key, "] invalid session");
				link->Close(ErrorCode::NullPointerError);
				return;
			}

			session->expire_time = ::time(nullptr);
			Singleton<Dispatcher<SESSION_T>>::GetInstance().OnRecvMsg(session, packet);
		}
		else
		{
			if(nullptr != handlers[msgID])
			{
				handlers[msgID](link, buffer);
			}
		}
	}

	std::shared_ptr<SESSION_T> FindSession(uint32_t session_key) 
	{
		return std::static_pointer_cast<SESSION_T>(session_manager.Find(session_key));
	}
	
	void DestroySession(uint32_t session_key)
	{
		std::shared_ptr<Network::Session> session = session_manager.Find(session_key);
		if (nullptr == session)
		{
			LOG(WRN, "can not find session(session_key:", session_key, ")");
			return;
		}
		session->OnDestroy();
		session_manager.Remove(session_key);
	}

	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().RegisterHandler(msg_id, func, factory);
	}

	void Recv_Connect_Req(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) 
	{
		LOG(DEV, "[link_key:", link->link_key, "]");	
		Json::Value ans;
		ans["error_code"] = 0;
		ans["session_key"] = 0;
		ans["session_token"] = "";

		try 
		{
			std::shared_ptr<SESSION_T> session = session_pool.Create();
			if(nullptr == session)
			{
				throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "[link_key:", link->link_key, "] can not create session instance");
			}

			session->session_token = Session::GenerateSessionToken(session->session_key);
			session->OnCreate();
			link->AttachSession(session);
			session_manager.Add(session->session_key, session);
			session->OnAccept();

			ans["session_key"] = session->session_key;
			ans["session_token"] = session->session_token;
			LOG(DEV, "[link_key:", link->link_key, "] session_key:", session->session_key, ", session_token:", session->session_token);	
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			ans["error_code"] = e.error_code();
		}

		Json::StyledWriter writer;
		std::string str = writer.write(ans);

		Packet::Header header;
		header.msg_id = MSG_ID::MsgID_SvrCli_Connect_Ans;
		header.msg_seq = 1;
		header.length = (uint16_t)(Packet::HEADER_SIZE + str.length()+1);

		std::shared_ptr<Packet> ans_packet = Packet::Create();
		if(nullptr == ans_packet)
		{
			return;
		}
		ans_packet->Write(header, str.c_str(), str.length()+1);
		link->AsyncSend(ans_packet);
	}

	void Recv_Reconnect_Req(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer)
	{
		Json::Value req;
		Json::Value ans;
		ans["error_code"] = 0;
		try 
		{
			std::string json = std::string(buffer->ReadPtr() + Packet::HEADER_SIZE, buffer->Size());
			Json::Reader reader;
			if (false == reader.parse(json, req))
			{
				throw Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "[link_key:", link->link_key, "] message format error(data:", json, ")");
			}

			uint32_t session_key = req["session_key"].asUInt();
			const std::string session_token = req["session_token"].asString();
				
			const std::shared_ptr<SESSION_T> session = Singleton<LinkManager<SESSION_T>>::GetInstance().FindSession(session_key);
			if (nullptr == session)
			{
				throw Exception(GAMNET_ERRNO(ErrorCode::InvalidSessionKeyError), "[link_key:", link->link_key, " ] can not find session data for reconnect(session_key:", session_key, ")");
			}

			if(session_token != session->session_token)
			{
				throw Exception(GAMNET_ERRNO(ErrorCode::InvalidSessionTokenError), "[link_key:", link->link_key, "] invalid session token(expect:", session->session_token, ", receive:", session_token, ")");
			}
				
			std::shared_ptr<Network::Tcp::Link> other = std::static_pointer_cast<Network::Tcp::Link>(session->link);
			if (nullptr != other)
			{
				other->strand.wrap([link, other] () {
					//link->recv_packet = other->recv_packet;
					std::static_pointer_cast<Tcp::Link>(link)->msg_seq = other->msg_seq;
					other->AttachSession(nullptr);
				});
			}

			link->AttachSession(session);
			session->OnAccept();
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			ans["error_code"] = e.error_code();
		}

		Json::StyledWriter writer;
		std::string str = writer.write(ans);

		Packet::Header header;
		header.msg_id = MSG_ID::MsgID_SvrCli_Reconnect_Ans;
		header.msg_seq = std::static_pointer_cast<Tcp::Link>(link)->msg_seq;
		header.length = (uint16_t)(Packet::HEADER_SIZE + str.length()+1);

		std::shared_ptr<Packet> ans_packet = Packet::Create();
		if(nullptr == ans_packet)
		{
			return;
		}
		ans_packet->Write(header, str.c_str(), str.length()+1);
		link->AsyncSend(ans_packet);
	}

	void Recv_HeartBeat_Req(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer)
	{
		Json::Value ans;
		ans["error_code"] = 0;
		ans["msg_seq"] = std::static_pointer_cast<Tcp::Link>(link)->msg_seq;

		Json::StyledWriter writer;
		std::string str = writer.write(ans);

		Packet::Header header;
		header.msg_id = MSG_ID::MsgID_SvrCli_HeartBeat_Ans;
		header.msg_seq = std::static_pointer_cast<Tcp::Link>(link)->msg_seq;
		header.length = (uint16_t)(Packet::HEADER_SIZE + str.length()+1);

		std::shared_ptr<Packet> ans_packet = Packet::Create();
		if(nullptr == ans_packet)
		{
			return;
		}
		ans_packet->Write(header, str.c_str(), str.length()+1);
		link->AsyncSend(ans_packet);
	}

	void Recv_Close_Ntf(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer)
	{
		std::shared_ptr<Network::Session> session = link->session;
		link->Close(ErrorCode::Success);
		if(nullptr == session)
		{
			LOG(DEV, "[link_key:", link->link_key, "] null session");
			return;
		}
		Singleton<LinkManager<SESSION_T>>::GetInstance().DestroySession(session->session_key);
	}

	Json::Value State()
	{
		Json::Value root = Network::LinkManager::State();
		root["name"] = _name;
		
		Json::Value session;
		session["capacity"] = (unsigned int)session_pool.Capacity();
		session["available"] = (unsigned int)session_pool.Available();
		session["running_count"] = (unsigned int)session_manager.Size();
		session["keepalive_time"] = session_manager.keepalive_time;
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
