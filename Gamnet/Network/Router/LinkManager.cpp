/*
 * RouterLinkManager.cpp
 *
 *  Created on: Jun 14, 2013
 *      Author: kukuta
 */

#include "LinkManager.h"
#include "RouterHandler.h"
#include "../Tcp/Tcp.h"

namespace Gamnet { namespace Network { namespace Router {

std::function<void(const Address& addr)> LinkManager::onRouterAccept = [](const Address&) {};
std::function<void(const Address& addr)> LinkManager::onRouterClose = [](const Address&) {};
std::mutex LinkManager::lock;

LinkManager::LinkManager()
{
}

LinkManager::~LinkManager() {
}

void LinkManager::Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose)
{
	LinkManager::onRouterAccept = onAccept;
	LinkManager::onRouterClose = onClose;
	RegisterHandler(
		MsgRouter_SetAddress_Req::MSG_ID,
		&RouterHandler::Recv_SetAddress_Req,
		new Network::HandlerStatic<RouterHandler>()
	);

	RegisterHandler(
		MsgRouter_SetAddress_Ans::MSG_ID,
		&RouterHandler::Recv_SetAddress_Ans,
		new Network::HandlerStatic<RouterHandler>()
	);

	RegisterHandler(
		MsgRouter_SetAddress_Ntf::MSG_ID,
		&RouterHandler::Recv_SetAddress_Ntf,
		new Network::HandlerStatic<RouterHandler>()
	);

	RegisterHandler(
		MsgRouter_SendMsg_Ntf::MSG_ID,
		&RouterHandler::Recv_SendMsg_Ntf,
		new Network::HandlerStatic<RouterHandler>()
	);
	localAddr_.service_name = service_name;
	localAddr_.cast_type = ROUTER_CAST_UNI;
	localAddr_.id = Network::Tcp::GetLocalAddress().to_v4().to_ulong();
	if(0 == localAddr_.id)
	{
#ifdef _WIN32
		throw Exception(0, "[", __FILE__, ":", __FUNCTION__, "@", __LINE__, "] unique router id is not set");
#else
		throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] unique router id is not set");
#endif

	}
	Network::LinkManager::Listen(port, 4096, 0);
}

bool LinkManager::Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose)
{
	const std::shared_ptr<Network::Link> link = Create();
	if(NULL == link)
	{
		return false;
	}

	const std::shared_ptr<Session> session = session_pool.Create();
	if(NULL == session)
	{
		LOG(GAMNET_WRN, "can not create any more session");//(max:", 1024, ", current:", sessionManager_.Size(), ")");
		return false;
	}

	session->remote_address = &(link->remote_address);
	session->onRouterConnect = onConnect;
	session->onRouterClose = onClose;
	link->AttachSession(session);

	return link->Connect(host, port, timeout);
	return true;
}

void LinkManager::OnClose(const std::shared_ptr<Network::Link>& link, int reason)
{
	link->AttachSession(std::shared_ptr<Session>(NULL));
	Network::LinkManager::OnClose(link, reason);
}

}}}
