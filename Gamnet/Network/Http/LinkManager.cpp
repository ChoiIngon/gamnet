#include "LinkManager.h"
#include "Dispatcher.h"
#include "Request.h"
#include "../../Library/String.h"
#include <curl/curl.h>


namespace Gamnet { namespace Network { namespace Http {

LinkManager::LinkManager()
{
	name = "Gamnet::Network::Http::LinkManager";
}

LinkManager::~LinkManager() 
{
}

void LinkManager::OnAccept(const std::shared_ptr<Link>& link)
{
	const std::shared_ptr<Session> session = std::shared_ptr<Session>(new Session());
	if (nullptr == session)
	{
		LOG(GAMNET_ERR, "create session instance fail(link_key:", link->link_key, ")");
		link->OnError(ErrorCode::CreateInstanceFailError);
		return;
	}

	session->read_buffer = Buffer::Create();
	if (nullptr == session->read_buffer)
	{
		LOG(GAMNET_ERR, "can not create buffer");
		link->OnError(ErrorCode::CreateInstanceFailError);
		return;
	}
	
	link->AttachSession(session);
	session->OnAccept();
}

void LinkManager::OnClose(const std::shared_ptr<Link>& link, int reason)
{
	const std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	if (nullptr != session)
	{
		session->OnClose(reason);
		session->read_buffer = nullptr;
	}
	link->AttachSession(nullptr);
	Network::LinkManager::OnClose(link, reason);
}

void LinkManager::OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer)
{
	std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	if (NULL == session)
	{
		LOG(GAMNET_ERR, "invalid session(link_key:", link->link_key, ")");
		link->OnError(ErrorCode::InvalidSessionError);
		return;
	}
	session->read_buffer->Append(buffer->ReadPtr(), buffer->Size());
	const std::string request = std::string(session->read_buffer->ReadPtr(), session->read_buffer->Size());
	size_t request_end = request.find("\r\n");
	if(std::string::npos == request_end)
	{
		return;
	}
	const std::string init_line = request.substr(0, request_end);
	std::string uri = init_line.substr(init_line.find("/") + 1);
	size_t uri_end = uri.find("?");
	size_t param_start = uri_end;
	size_t param_end = uri.find(" ");
	if (std::string::npos == uri_end)
	{
		uri_end = param_end;
	}
	std::string param;
	if (std::string::npos != param_start)
	{
		param = uri.substr(param_start + 1, param_end - (param_start + 1));
		CURL* curl_ = curl_easy_init();
		if (NULL == curl_)
		{
			throw Exception(ERR, "curl lib isn't inited");
		}
		char* plain_text = curl_easy_unescape(curl_, uri.substr(param_start + 1, param_end - (param_start + 1)).c_str(), 0, NULL);
		param = plain_text;
		curl_free(plain_text);
		curl_easy_cleanup(curl_);
	}
	uri = uri.substr(0, uri_end);
	Request req(param);
	Singleton<Dispatcher>::GetInstance().OnRecvMsg(link, uri, req);
	link->OnError(ErrorCode::Success);
}

}}}
