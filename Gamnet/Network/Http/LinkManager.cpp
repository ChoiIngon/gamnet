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
		LOG(GAMNET_ERR, "can not create 'Http::Link' instance");
		return nullptr;
	}

	return link;
}

void LinkManager::OnAccept(const std::shared_ptr<Network::Link>& link)
{
	const std::shared_ptr<Session> session = std::shared_ptr<Session>(new Session());
	if (nullptr == session)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "[link_key:", link->link_key, "] can not create session instance");
	}

	session->session_key = ++Network::Session::session_key_generator;

	link->session = session;
	
	session->strand.wrap(std::bind(&Session::OnCreate, session))();
	session->strand.wrap(std::bind(&Session::AttachLink, session, link))();
	session->strand.wrap(std::bind(&Session::OnAccept, session))();
}

void LinkManager::OnClose(const std::shared_ptr<Network::Link>& link, int reason)
{
	std::shared_ptr<Network::Http::Session> session = std::static_pointer_cast<Network::Http::Session>(link->session);
	if (nullptr == session)
	{
		LOG(ERR, "[link_key:", link->link_key, "] link refers invalid session");
		return;
	}
	
	session->strand.wrap(std::bind(&Session::OnClose, session, reason))();
	session->strand.wrap(std::bind(&Session::AttachLink, session, nullptr))();
	session->strand.wrap(std::bind(&Session::OnDestroy, session))();
}


}}}
