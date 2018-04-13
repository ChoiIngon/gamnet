#include "LinkManager.h"
#include "RouterHandler.h"
#include "../Tcp/Tcp.h"

namespace Gamnet { namespace Network { namespace Router {

std::function<void(const Address& addr)> LinkManager::onRouterAccept = [](const Address&) {};
std::function<void(const Address& addr)> LinkManager::onRouterClose = [](const Address&) {};
std::mutex LinkManager::lock;

LinkManager::LinkManager()
{
	_name = "Gamnet::Network::Router::LinkManager";
	_cast_group = Tcp::CastGroup::Create();
}

LinkManager::~LinkManager() {
}

void LinkManager::Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose)
{
	LinkManager::onRouterAccept = onAccept;
	LinkManager::onRouterClose = onClose;

	RegisterHandler(MsgRouter_SetAddress_Req::MSG_ID, &RouterHandler::Recv_SetAddress_Req, new Network::HandlerStatic<RouterHandler>());
	RegisterHandler(MsgRouter_SetAddress_Ans::MSG_ID, &RouterHandler::Recv_SetAddress_Ans, new Network::HandlerStatic<RouterHandler>());
	RegisterHandler(MsgRouter_SetAddress_Ntf::MSG_ID, &RouterHandler::Recv_SetAddress_Ntf, new Network::HandlerStatic<RouterHandler>());
	RegisterHandler(MsgRouter_SendMsg_Ntf::MSG_ID, &RouterHandler::Recv_SendMsg_Ntf, new Network::HandlerStatic<RouterHandler>());
	RegisterHandler(MsgRouter_HeartBeat_Ntf::MSG_ID, &RouterHandler::Recv_HeartBeat_Ntf, new Network::HandlerStatic<RouterHandler>());
	local_address.service_name = service_name;
	local_address.cast_type = ROUTER_CAST_TYPE::UNI_CAST;
	local_address.id = Network::Tcp::GetLocalAddress().to_v4().to_ulong();
	if(0 == local_address.id)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, "unique router id is not set");
	}

	_heartbeat_timer.AutoReset(true);
	_heartbeat_timer.SetTimer(60000, [this] () {
		std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
		if(NULL != packet) {
			MsgRouter_HeartBeat_Ntf ntf;
			_cast_group->SendMsg(ntf);
			LOG(DEV, "[Router] send heartbeat message(link count:", _cast_group->Size(), ")");
		}
	});

	session_manager.Init(0);
	Network::LinkManager::Listen(port);
}

void LinkManager::Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose)
{
	std::shared_ptr<Network::Link> link = Create();
	if(nullptr == link)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "cannot create link instance");
	}

	std::shared_ptr<Session> session = session_pool.Create();
	if (nullptr == session)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "cannot create session instance");
	}

	link->session = session;
	session->onRouterConnect = onConnect;
	session->onRouterClose = onClose;

	session_manager.Add(session->session_key, session);

	session->strand.wrap(std::bind(&Session::OnCreate, session))();
	session->strand.wrap(std::bind(&Session::AttachLink, session, link))();
	
	link->Connect(host, port, timeout);
	std::lock_guard<std::mutex> lo(this->_lock);
	_links.insert(std::make_pair(link->link_key, link));
}

void LinkManager::OnConnect(const std::shared_ptr<Network::Link>& link)
{
	const std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	if (nullptr == link->session)
	{
		link->Close(ErrorCode::InvalidSessionError);
		return;
	}
	
	session->strand.wrap(std::bind(&Session::OnConnect, session))();
	_cast_group->AddSession(session);
}

void LinkManager::OnAccept(const std::shared_ptr<Network::Link>& link)
{
	std::shared_ptr<Session> session = session_pool.Create();
	if (nullptr == session)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "[link_key:", link->link_key, "] can not create session instance");
	}

	link->session = session;

	_cast_group->AddSession(session);

	session_manager.Add(session->session_key, session);

	session->strand.wrap(std::bind(&Session::OnCreate, session))();
	session->strand.wrap(std::bind(&Session::AttachLink, session, link))();
	session->strand.wrap(std::bind(&Session::OnAccept, session))();
	
	
}

void LinkManager::OnClose(const std::shared_ptr<Network::Link>& link, int reason)
{
	const std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	if (nullptr == session)
	{
		return;
	}
	session->strand.wrap(std::bind(&Session::OnClose, session, reason))();
	session->strand.wrap(std::bind(&Session::AttachLink, session, nullptr))();
	session->strand.wrap(std::bind(&Session::OnDestroy, session))();

	session_manager.Remove(session->session_key);

	_cast_group->DelSession(session);
}

void LinkManager::OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) 
{
	std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	if (nullptr == session)
	{
		return;
	}

	session->expire_time = ::time(nullptr);
	const std::shared_ptr<Tcp::Packet>& packet = std::static_pointer_cast<Tcp::Packet>(buffer);
	session->strand.wrap(std::bind(&Tcp::Dispatcher<Session>::OnRecvMsg, &Singleton<Tcp::Dispatcher<Session>>::GetInstance(), session, packet))();
}

}}}
