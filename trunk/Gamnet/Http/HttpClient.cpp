#include <curl/curl.h>
#include <algorithm>
#include <map>
#include <string>
#include <cstring>
#include "../Log/Log.h"
struct curl_slist;

#include "HttpClient.h"
#include <unistd.h>

namespace Gamnet { namespace Http {
static CURLcode globalCURL = curl_global_init(CURL_GLOBAL_ALL);

HttpClient::HttpClient(const char* host) 
	: host_(host)
{
	curl_ = curl_easy_init();
}

HttpClient::~HttpClient()
{
	if(NULL != curl_)
	{
		curl_easy_cleanup(curl_);
		curl_ = NULL;
	}
}

bool HttpClient::Get(const char* path, const char* param, std::function<void(int errcode, const char* data)> callback)
{
	if(NULL == curl_)
	{
		return false; //Exception(500, "curl lib isn't inited");
	}

	std::string path_param = path;
	if(NULL != param && 0 < strlen(param))
	{
		path_param +=  "?" + std::string(param);
	}

	int httpCode = 200;
	try {
		HttpRequest(path_param.c_str());
	}
	catch(const Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		return false;
	}
	callback(httpCode, resData_.c_str());
	return true;
}

bool HttpClient::Post(const char* path, const char* param, std::function<void(int errcode, const char* data)> callback)
{
	if(NULL == curl_)
	{
		return false; //Exception(500, "curl lib isn't inited");
	}

	int httpCode = 200;
	try {
		int nPostFieldSize = 0;
		if(NULL != param && 0 < strlen(param))
		{
			if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, param))
			{
				throw Exception(500, "set post field error");
			}
			nPostFieldSize = strlen(param);
		}

		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, nPostFieldSize))
		{
			throw Exception(500, "set post field size error");
		}

		httpCode = HttpRequest(path);
	}
	catch(const Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		return false;
	}
	callback(httpCode, resData_.c_str());
	return true;
}

int HttpClient::HttpRequest(const char* path)
{
	curl_slist *header_list=NULL;
	long _httpCode = 200;

	try {
		if(NULL == curl_)
		{
			throw Exception(500, "curl lib isn't inited");
		}

		for(auto itr : mapHeader_)
		{
			std::string header = itr.first + ": " + itr.second;
			header_list = curl_slist_append(header_list, header.c_str());
		}

		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_list))
		{
			throw Exception(500, "set header error");
		}

		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1))
		{
			throw Exception(500, "set no signal option error");
		}

		const std::string url = host_ + "/" + std::string(path);
		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_URL, url.c_str()))
		{
			throw Exception(500, "set url error");
		}

		std::string protocol = host_.substr(0, host_.find("://"));
		std::transform(protocol.begin(), protocol.end(), protocol.begin(), ::toupper);
		if("HTTPS" == protocol)
		{
			if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 1L))
			{
				throw Exception(500, "set ssl option error");
			}
		}

		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)this))
		{
			throw Exception(500, "set callback arg error");
		}

		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, Callback))
		{
			throw Exception(500, "set callback function error");
		}

		while(true)
		{
			if(CURLE_OK != curl_easy_perform(curl_))
			{
				throw Exception(500, "perform http request error");
			}

			if(CURLE_OK != curl_easy_getinfo(curl_, CURLINFO_HTTP_CODE, &_httpCode))
			{
				throw Exception(500, "get return error code error");
			}

			if(3 == _httpCode / 100)
			{
				curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
				continue;
			}
			break;
		}
	}
	catch(const Exception& e)
	{
		_httpCode = e.error_code();
		Log::Write(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	if(NULL != header_list)
	{
		curl_slist_free_all(header_list);
		header_list = NULL;
	}

	return _httpCode;
}

size_t HttpClient::Callback(void *ptr, size_t size, size_t nmemb, void *arg)
{
	const size_t byte_size_of_ptr = size * nmemb;
	HttpClient* self = (HttpClient*)arg;
	self->resData_ += (const char*)ptr;
	return byte_size_of_ptr;
}

}};
