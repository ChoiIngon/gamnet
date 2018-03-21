#include "LinkManager.h"
#include "Dispatcher.h"
#include "Request.h"
#include "../../Library/String.h"
#include <curl/curl.h>


namespace Gamnet { namespace Network { namespace Http {

LinkManager::LinkManager() : link_pool(65535, LinkFactory(this))
{
	_name = "Gamnet::Network::Http::LinkManager";
}

LinkManager::~LinkManager() 
{
}

std::shared_ptr<Network::Link> LinkManager::Create() 
{
	std::shared_ptr<Link> link = link_pool.Create();
	if(nullptr == link)
	{
		LOG(GAMNET_ERR, "can not create 'Tcp::Link' instance");
		return nullptr;
	}

	const std::shared_ptr<Session> session = std::shared_ptr<Session>(new Session());
	if (nullptr == session)
	{
		LOG(GAMNET_ERR, "create session instance fail(link_key:", link->link_key, ")");
		return nullptr;
	}

	session->session_key = ++Network::SessionManager::session_key;
		
	link->session = session;
	session->link = link;
	session->remote_address = &(link->remote_address);
	session->OnCreate();
	return link;
}

void LinkManager::OnAccept(const std::shared_ptr<Network::Link>& link)
{
	std::shared_ptr<Network::Session> session = link->session;
	session->OnAccept();
}

void LinkManager::OnClose(const std::shared_ptr<Network::Link>& link, int reason)
{
	std::shared_ptr<Network::Session> session = link->session;
	if (nullptr == session)
	{
		LOG(ERR, "[link_key:", link->link_key, "] link refers invalid session");
		return;
	}

	session->OnClose(reason);
	session->OnDestroy();
}

}}}
