#include "LinkManager.h"
#include "RouterHandler.h"
#include "../Tcp/Tcp.h"

namespace Gamnet { namespace Network { namespace Router {

LinkManager::LinkManager() : heartbeat_group(Tcp::CastGroup::Create())
{
	name = "Gamnet::Network::Router::LinkManager";
}

LinkManager::~LinkManager() {
}

void LinkManager::Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose, int accept_queue_size)
{
	local_address.service_name = service_name;
	local_address.cast_type = ROUTER_CAST_TYPE::UNI_CAST;
	local_address.id = Network::Tcp::GetLocalAddress().to_v4().to_ulong();
	if(0 == local_address.id)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, "unique router id is not set");
	}

	this->port = port;
	this->on_connect = onConnect;
	this->on_close = onClose;

	heartbeat_timer.AutoReset(true);
	heartbeat_timer.SetTimer(60000, [this] () {
		MsgRouter_HeartBeat_Ntf ntf;
		Tcp::CastGroup::LockGuard lockedCastGroup(heartbeat_group);
		LOG(GAMNET_INF, "[Router] send heartbeat message(connected server count:", lockedCastGroup->Size(), ")");
		lockedCastGroup->SendMsg(ntf);
	});

	session_manager.Init();
	Network::Tcp::LinkManager<Session>::Listen(port, 1024, 0, accept_queue_size);
}

void LinkManager::Connect(const char* host, int port, int timeout)
{
	std::shared_ptr<Network::Tcp::Link> link = std::make_shared<Network::Tcp::Link>(this);
	if(nullptr == link)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "cannot create link instance");
	}
	link->Init();

	std::shared_ptr<Session> session = session_pool.Create();
	if (nullptr == session)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "cannot create session instance");
	}

	link->session = session;
	session->link = link;
	session->OnCreate();
	
	session_manager.Add(session->session_key, session);
	link->Connect(host, port, timeout);
}

void LinkManager::OnConnect(const std::shared_ptr<Network::Link>& link)
{
	std::shared_ptr<Network::Tcp::Link> tcpLink = std::static_pointer_cast<Network::Tcp::Link>(link);
	const std::shared_ptr<Session> session = std::static_pointer_cast<Session>(tcpLink->session);
	assert(nullptr != session);

	if (false == Add(tcpLink))
	{
		assert(!"duplicated link");
		throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "duplicated link");
	}

	LOG(INF, "[Router-", link->link_key, "-", session->session_key, "] connect success..(remote ip:", session->GetRemoteAddress().to_string(), ")");
	LOG(INF, "[Router-", link->link_key, "-", session->session_key, "] localhost->", session->GetRemoteAddress().to_string(), " SEND MsgRouter_SetAddress_Req(router_address:", Singleton<LinkManager>::GetInstance().local_address.ToString(), ")");
	MsgRouter_SetAddress_Req req;
	req.router_address = Singleton<LinkManager>::GetInstance().local_address;
	req.host = Network::Tcp::GetLocalAddress().to_string();
	req.port = Singleton<LinkManager>::GetInstance().port;
	Network::Tcp::SendMsg(session, req, false);
}

void LinkManager::OnAccept(const std::shared_ptr<Network::Link>& link)
{
	std::shared_ptr<Network::Tcp::Link> tcpLink = std::static_pointer_cast<Network::Tcp::Link>(link);
	std::shared_ptr<Session> session = session_pool.Create();
	if (nullptr == session)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "[link_key:", link->link_key, "] can not create session instance");
	}

	tcpLink->session = session;
	session->link = tcpLink;
	session->OnCreate();
	session->OnAccept();
	
	session_manager.Add(session->session_key, session);
	if (false == Add(tcpLink))
	{
		assert(!"duplicated link");
		throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "duplicated link");
	}
}

void LinkManager::OnClose(const std::shared_ptr<Network::Link>& link, int reason)
{
	std::shared_ptr<Network::Tcp::Link> tcpLink = std::static_pointer_cast<Network::Tcp::Link>(link);
	const std::shared_ptr<Session> session = std::static_pointer_cast<Session>(tcpLink->session);
	if (nullptr == session)
	{
		return;
	}

	session_manager.Remove(session->session_key);
	session->OnClose(reason);
	session->OnDestroy();
	session->link = nullptr;
	tcpLink->session = nullptr;

	Remove(tcpLink->link_key);
	Tcp::CastGroup::LockGuard lockedCastGroup(heartbeat_group);
	lockedCastGroup->Remove(session);
}

void LinkManager::OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) 
{
	std::shared_ptr<Network::Tcp::Link> tcpLink = std::static_pointer_cast<Network::Tcp::Link>(link);
	if (nullptr == tcpLink->session)
	{
		return;
	}

	Singleton<Tcp::Dispatcher<Session>>::GetInstance().OnRecvMsg(tcpLink, buffer);
}

Json::Value LinkManager::State()
{
	Json::Value root;
	root["name"] = name;

	Json::Value session;
	session["max_count"] = (unsigned int)session_pool.Capacity();
	session["idle_count"] = (unsigned int)session_pool.Available();
	session["active_count"] = (unsigned int)session_manager.Size();
	root["session"] = session;
	return root;
}
}}}
