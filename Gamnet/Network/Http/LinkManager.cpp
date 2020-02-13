#include "LinkManager.h"
#include "Dispatcher.h"
#include "Request.h"
#include "../../Library/String.h"
#include <curl/curl.h>


namespace Gamnet { namespace Network { namespace Http {

LinkManager::LinkManager() : link_pool(65535, LinkFactory(this))
{
	name = "Gamnet::Network::Http::LinkManager";
}

LinkManager::~LinkManager() 
{
}

std::shared_ptr<Network::Link> LinkManager::Create() 
{
	std::shared_ptr<Link> link = link_pool.Create();
	if(nullptr == link)
	{
		LOG(ERR, "[", name, "/0/0] can not create link instance");
		return nullptr;
	}

	std::shared_ptr<Session> session = std::shared_ptr<Session>(new Session());
	if (nullptr == session)
	{
		LOG(ERR, "[", name, "/", link->link_key, "/0] can not create session instance");
		return nullptr;
	}

	session->Init();
	session->link = link;
	link->session = session;
	session->OnCreate();
	return link;
}

void LinkManager::OnAccept(const std::shared_ptr<Network::Link>& link)
{
	assert(nullptr != link->session);
	link->session->OnAccept();
}

void LinkManager::OnClose(const std::shared_ptr<Network::Link>& link, int reason)
{
	std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	assert(nullptr != link->session);
	
	session->OnClose(reason);
	session->OnDestroy();
	session->link = nullptr;
}

}}}
