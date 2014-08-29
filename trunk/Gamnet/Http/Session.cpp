/*
 * Session.cpp
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#include "Session.h"
#include "Dispatcher.h"
#include "../Library/String.h"
#include <curl/curl.h>
namespace Gamnet {
namespace Http {

Session::Session() {
}

Session::~Session() {
}

void Session::AsyncRead()
{
	auto self(std::static_pointer_cast<Session>(shared_from_this()));
	socket_.async_read_some(boost::asio::buffer(readBuffer_->WritePtr(), readBuffer_->Available()),
		strand_.wrap([self](boost::system::error_code ec, std::size_t readbytes) {
			if (0 != ec)
			{
				self->OnError(errno); // no error, just closed socket
				return;
			}
			for(size_t readPtr=0; readPtr<readbytes; readPtr++)
			{
				if('\r' == *(self->readBuffer_->WritePtr() + readPtr) &&
				   '\n' == *(self->readBuffer_->WritePtr() + readPtr+1) )
				{
					const std::string request = self->readBuffer_->ReadPtr();
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

					Singleton<Dispatcher>::GetInstance().OnRecvMsg(self, uri, mapParam);
					self->OnError(errno); // no error, just closed socket
					return;
				}
			}
			self->readBuffer_->writeCursor_ += readbytes;
			self->AsyncRead();
		})
	);
}

int Session::Send(const Response& res)
{
	std::string response;
	response += Format("HTTP/1.1 ", res.nErrorCode, " ", GetErrorStr(res.nErrorCode), "\r\n");
	response += Format("\r\n");
	response += res.sBodyContext;
	response += Format("\r\n\r\n");
	return Network::Session::Send(response.c_str(), response.length());
}
} /* namespace Http */
} /* namespace Gamnet */
