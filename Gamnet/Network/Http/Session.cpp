#include "Request.h"
#include "Dispatcher.h"
#include "Session.h"
//#include "../../Library/String.h"
#include "../../Library/Singleton.h"
#include <curl/curl.h>

import Gamnet.String;

namespace Gamnet { namespace Network { namespace Http {

Session::Session() {
}

Session::~Session() {
}

bool Session::Init()
{
	if(false == Network::Session::Init())
	{
		return false;
	}
	recv_buffer = Buffer::Create();
	return true;
}

void Session::Send(const Response& res)
{
	std::string response;
	response += Format("HTTP/1.1 ", res.error_code, " ", GetErrorStr(res.error_code), "\r\n");
	response += Format("\r\n");
	response += res.context;
	response += Format("\r\n\r\n");
	AsyncSend(response.c_str(), response.length());
}

void Session::OnRead(const std::shared_ptr<Buffer>& buffer) 
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
	Singleton<Dispatcher>::GetInstance().OnReceive(std::static_pointer_cast<Session>(self), uri, req);
	Close(ErrorCode::Success);
}

void Session::Close(int reason)
{
	if (nullptr == socket)
	{
		return;
	}
	OnClose(reason);
	socket = nullptr;
}

}}}
