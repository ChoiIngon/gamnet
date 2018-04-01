#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "LinkManager.h"
#include "RouterCaster.h"


namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

/*
Session* Session::Init::operator() (Session* session)
{
	return session;
}
*/

Session::Session() : Network::Tcp::Session() {
	onRouterConnect = [](const Address&) {};
	onRouterClose = [](const Address&) {};
}

Session::~Session() {}

void Session::OnCreate() {
}

void Session::OnAccept() {
	onRouterClose = LinkManager::onRouterClose;
}

void Session::OnConnect()
{
	watingSessionManager_.Clear();
	LOG(GAMNET_INF, "[Router] connect success..(remote ip:", remote_address->to_string(), ")");
	MsgRouter_SetAddress_Req req;
	req.local_address = Singleton<LinkManager>::GetInstance().local_address;
	Network::Tcp::SendMsg(std::static_pointer_cast<Session>(shared_from_this()), req);
	LOG(GAMNET_INF, "[Router] send SetAddress_Req (localhost->", remote_address->to_string(), ", service_name:", req.local_address.service_name.c_str(), ")");
}

void Session::OnClose(int reason)
{
	LOG(GAMNET_INF, "[Router] remote server closed(session_key:", session_key, ", ip:", remote_address->to_string(), ", service_name:", address.service_name, ")");
	if("" != address.service_name)
	{
		{
			std::lock_guard<std::mutex> lo(LinkManager::lock);
			onRouterClose(address);
		}
		Singleton<RouterCaster>::GetInstance().UnregisterAddress(address);
	}
	watingSessionManager_.Clear();
}

void Session::OnDestroy()
{
}

}}} /* namespace Gamnet */
