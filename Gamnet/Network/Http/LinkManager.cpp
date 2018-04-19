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
	
	session->strand.wrap([session, link]() {
		try {
			session->OnCreate();
			session->AttachLink(link);
			session->OnAccept();
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		}
	})();
}

void LinkManager::OnClose(const std::shared_ptr<Network::Link>& link, int reason)
{
	std::shared_ptr<Network::Http::Session> session = std::static_pointer_cast<Network::Http::Session>(link->session);
	if (nullptr == session)
	{
		LOG(ERR, "[link_key:", link->link_key, "] link refers invalid session");
		return;
	}
	
	session->strand.wrap([session, reason]() {
		try {
			session->OnClose(reason);
			session->AttachLink(nullptr);
			session->OnDestroy();
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		}
	})();
}


}}}
