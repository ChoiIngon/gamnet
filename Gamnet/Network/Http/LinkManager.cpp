#include "LinkManager.h"
#include "Dispatcher.h"
#include "../../Library/String.h"
#include <curl/curl.h>


namespace Gamnet { namespace Network { namespace Http {

LinkManager::LinkManager()
{
}

LinkManager::~LinkManager() {
}

void LinkManager::OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer)
{
	for(size_t read_ptr=0; read_ptr<buffer->Size(); read_ptr++)
	{
		if('\r' == *(buffer->ReadPtr() + read_ptr) &&
		   '\n' == *(buffer->ReadPtr() + read_ptr+1) )
		{
			const std::string request = std::string(buffer->ReadPtr(), read_ptr+1);
			const std::string init_line = request.substr(0, request.find("\n"));
			std::string uri = init_line.substr(init_line.find("/")+1);
			size_t uri_end = uri.find("?");
			size_t param_start = uri_end;
			size_t param_end = uri.find(" ");
			if(std::string::npos == uri_end)
			{
				uri_end = param_end;
			}
			std::string param;
			if(std::string::npos != param_start)
			{
				CURL* curl_ = curl_easy_init();
				if(NULL == curl_)
				{
					throw Exception(ERR, "curl lib isn't inited");
				}
				char* plain_text = curl_easy_unescape(curl_, uri.substr(param_start+1, param_end-(param_start+1)).c_str(), 0, NULL);
				param = plain_text;
				curl_free(plain_text);
				curl_easy_cleanup(curl_);
			}
			uri = uri.substr(0, uri_end);

			std::map<std::string, std::string> mapParam;
			CURL* curl_ = curl_easy_init();
			if(NULL == curl_)
			{
				throw Exception(ERR, "curl lib isn't inited");
			}
			while(0 < param.length())
			{
				size_t equal_pos = param.find("=");
				if(std::string::npos == equal_pos)
				{
					break;
				}
				size_t delim_pos = param.find("&");
				if(std::string::npos == delim_pos)
				{
					delim_pos = param.length();
				}

				std::string name = param.substr(0, equal_pos);
				std::string value = param.substr(equal_pos+1, delim_pos-(equal_pos+1));
				mapParam.insert(std::make_pair(name, value));
				if(delim_pos >= param.length())
				{
					break;
				}
				param = param.substr(delim_pos+1);
			}

			const std::shared_ptr<Session> session = std::shared_ptr<Session>(new Session());
			link->AttachSession(session);
			Singleton<Dispatcher>::GetInstance().OnRecvMsg(link, uri, mapParam);
			link->OnError(errno); // no error, just closed socket
			return;
		}
	}
}

}}}
