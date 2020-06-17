#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include "RouterCaster.h"
#include <boost/bind.hpp>

namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Session::Session() 
	: Network::Tcp::Session()
{
}

Session::~Session() 
{
}

void Session::OnCreate() 
{
}

void Session::OnAccept() 
{
}

void Session::OnConnect()
{	
	static_cast<SessionManager*>(session_manager)->on_connect(router_address);
}

void Session::OnClose(int reason)
{
	//LOG(INF, "[", session_key, "] remote server closed(ip:", GetRemoteAddress().to_string(), ", service_name:", address.service_name, ", reason:", reason, ")");
	if("" != router_address.service_name)
	{
		Singleton<RouterCaster>::GetInstance().UnregisterAddress(router_address);
		static_cast<SessionManager*>(session_manager)->on_close(router_address);
	}
}

void Session::OnDestroy()
{
}

void Session::Close(int reason)
{
	if(nullptr == socket)
	{
		return;
	}

	OnClose(reason);
	socket = nullptr;

	OnDestroy();
	session_manager->Remove(shared_from_this());
}

void LocalSession::AsyncSend(const std::shared_ptr<Tcp::Packet> packet)
{
	auto self(shared_from_this());
	strand->wrap(boost::bind(&Network::SessionManager::OnReceive, session_manager, self, packet))();
}
}}} /* namespace Gamnet */
