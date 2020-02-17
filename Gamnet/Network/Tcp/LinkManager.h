#ifndef GAMNET_NETWORK_TCP_LINKMANAGER_H_
#define GAMNET_NETWORK_TCP_LINKMANAGER_H_

#include "Dispatcher.h"
#include "Packet.h"
#include "Link.h"
#include "SystemMessageHandler.h"
#include "../LinkManager.h"
#include "../../Library/Json/json.h"
#include "../Acceptor.h"
#include <memory>
#include <atomic>
#include <mutex>

namespace Gamnet { namespace Network { namespace Tcp {
	
template <class SESSION_T>
class LinkManager : public Network::LinkManager
{
private :
	Acceptor<Link> acceptor;
	struct LinkFactory {
		LinkManager* const link_manager;
		LinkFactory(LinkManager* linkManager) : link_manager(linkManager) {}

		Link* operator() ()
		{
			return new Link(link_manager);
		}
	};
	
public :
	SessionManager session_manager;
	Pool<SESSION_T, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> session_pool;
	Pool<Link, std::mutex, Network::Link::InitFunctor, Network::Link::ReleaseFunctor> link_pool;

	LinkManager() : acceptor(65535, LinkFactory(this)), session_pool(), link_pool(65535, LinkFactory(this))
	{
		name = "Gamnet::Network::Tcp::LinkManager";
	}

	virtual ~LinkManager()	
	{
	}

	void Listen(int port, int max_session, int keep_alive_time, int accept_queue_size)
	{
		BindHandler(MSG_ID::MsgID_CliSvr_Connect_Req, "MsgID_CliSvr_Connect_Req", &SystemMessageHandler<SESSION_T>::Recv_Connect_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		BindHandler(MSG_ID::MsgID_CliSvr_Reconnect_Req, "MsgID_CliSvr_Reconnect_Req", &SystemMessageHandler<SESSION_T>::Recv_Reconnect_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		BindHandler(MSG_ID::MsgID_CliSvr_Close_Req, "MsgID_CliSvr_Close_Req", &SystemMessageHandler<SESSION_T>::Recv_Close_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		BindHandler(MSG_ID::MsgID_CliSvr_HeartBeat_Req, "MsgID_CliSvr_HeartBeat_Req", &SystemMessageHandler<SESSION_T>::Recv_HeartBeat_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		BindHandler(MSG_ID::MsgID_CliSvr_ReliableAck_Ntf, "MsgID_CliSvr_ReliableAck_Ntf", &SystemMessageHandler<SESSION_T>::Recv_ReliableAck_Ntf, new HandlerStatic<SystemMessageHandler<SESSION_T>>());

		link_pool.Capacity(max_session);
		session_pool.Capacity(65535);
		session_manager.Init();

		acceptor.Listen(port, max_session, accept_queue_size);
		//Network::LinkManager::Listen(port, accept_queue_size, keep_alive_time);
	}

	virtual std::shared_ptr<Network::Link> Create() override
	{
		std::shared_ptr<Link> link = link_pool.Create();
		if(nullptr == link) 
		{
			LOG(GAMNET_ERR, "[link_manager:", name, "] can not create 'Tcp::Link' instance");
			return nullptr;
		}
		return link;
	}

	virtual void OnAccept(const std::shared_ptr<Network::Link>& link) override
	{
		std::shared_ptr<SESSION_T> session = session_pool.Create();
		if (nullptr == session)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionError, "[", name, "/", link->link_key, "/0] can not create session instance(availble:", session_pool.Available(), ")");
		}

		session->link = link;
		link->session = session;
	}
	
	virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason) override
	{
		std::shared_ptr<Link> tcpLink = std::static_pointer_cast<Link>(link);
		std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(tcpLink->session);
		if(nullptr == session)
		{
			return;
		}

		session->OnClose(reason);
		if (false == session->handover_safe || (0 < tcpLink->link_manager->keep_alive_time && tcpLink->expire_time + tcpLink->link_manager->keep_alive_time < time(nullptr)))
		{
			session->OnDestroy();
			session->link = nullptr;
			tcpLink->session = nullptr;
			session_manager.Remove(session->session_key);
		}
	}

	virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) override
	{
		std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
		if (nullptr == session)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionError, "[", name, "/", link->link_key, "/0] invalid session");
		}

		Singleton<Dispatcher<SESSION_T>>::GetInstance().OnRecvMsg(link, buffer);
	}

	std::shared_ptr<SESSION_T> FindSession(uint32_t session_key) 
	{
		return std::static_pointer_cast<SESSION_T>(session_manager.Find(session_key));
	}
	
	void DestroySession(uint32_t session_key)
	{
		std::shared_ptr<SESSION_T> session = session_manager.Find(session_key);
		if (nullptr == session)
		{
			LOG(WRN, "can not find session(", name,"::session_key:", session_key, ")");
			return;
		}
	
		session_manager.Remove(session->session_key);
		assert(session->link);
		session->link->strand.wrap([=] () {
			session->handover_safe = false;
			link->Close(ErrorCode::Success);
		})();
	}

	template <class FUNC, class FACTORY>
	bool BindHandler(unsigned int msg_id, const char* name, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().BindHandler(msg_id, name, func, factory);
	}

	Json::Value State()
	{
		Json::Value root;
		root["name"] = name;

		Json::Value link;
		link["max_count"] = link_pool.Capacity();
		link["idle_count"] = link_pool.Available();
		link["active_count"] = Size();
		root["link"] = link;
		
		Json::Value session;
		session["max_count"] = session_pool.Capacity();
		session["idle_count"] = session_pool.Available();
		session["active_count"] = session_manager.Size();
		root["session"] = session;

		Json::Value packet;
		packet["max_count"] = Packet::PoolCapacity();
		packet["idle_count"] = Packet::PoolAvailable();
		packet["active_count"] = Packet::PoolSize();
		root["packet"] = packet;
#ifdef _DEBUG
		Json::Value message;
		for (const auto& itr : Singleton<Dispatcher<SESSION_T>>::GetInstance().mapHandlerCallStatistics_)
		{
			time_t now = time(nullptr);
			itr.second->last_check_time = now;
			itr.second->last_finish_count = itr.second->finish_count;
			
			Json::Value statistics;
			statistics["msg_id"] = (int)itr.second->msg_id;
			statistics["msg_name"] = (const char*)itr.second->name;
			statistics["begin_count"] = (int)itr.second->begin_count;
			statistics["finish_count"] = (int)itr.second->finish_count;
			statistics["elapsed_time"] = (int)itr.second->elapsed_time;
			statistics["max_time"] = (int)itr.second->max_time;
			
			message.append(statistics);
		}
		root["message"] = message;
#endif
		return root;
	}
};

}}}
#endif /* LISTENER_H_ */
