#include "Link.h"
#include "Request.h"
#include "Dispatcher.h"
#include <curl/curl.h>

namespace Gamnet { namespace Network { namespace Http {

Link::Link(Network::LinkManager* linkManager) : Network::Link(linkManager)
{
}

Link::~Link()
{
}

bool Link::Init()
{
	if (false == Network::Link::Init())
	{
		return false;
	}

	recv_buffer = Buffer::Create();
	if (nullptr == recv_buffer)
	{
		LOG(GAMNET_ERR, "[link_key:", link_key, "] can not create recv packet");
		return false;
	}
	return true;
}

void Link::OnRead(const std::shared_ptr<Buffer>& buffer)
{
	recv_buffer->Append(buffer->ReadPtr(), buffer->Size());
	const std::string request = std::string(recv_buffer->ReadPtr(), recv_buffer->Size());

	size_t request_end = request.find("\r\n");
	if (std::string::npos == request_end)
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
		if (nullptr == curl_)
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
	auto self = shared_from_this();
	Singleton<Dispatcher>::GetInstance().OnRecvMsg(self, uri, req);
	Close(ErrorCode::Success);
}
}}}