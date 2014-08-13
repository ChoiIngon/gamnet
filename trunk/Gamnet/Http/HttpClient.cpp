#include <curl/curl.h>
#include <json/json.h>
#include <algorithm>
#include <map>
#include <string>
#include <cstring>

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

/*
void HttpClient::ParseJSON(std::string& param, const char* key, json_object* jobj)
{
	if(NULL == jobj)
	{
		return ;
	}
	const json_type type = json_object_get_type(jobj);
	if(type == json_type_object)
	{
		json_object_object_foreach(jobj, key, val)
		{
			ParseJSON(param, key, val);
		}
	}
	else
	{
		switch(type) 
		{
			case json_type_boolean :
			case json_type_double :
			case json_type_int :
			case json_type_string :
				if(0 < param.length())
				{
					param += "&";
				}
				{
					char* encodedKey = curl_easy_escape(curl_, key, 0);
					char* encodedVal = curl_easy_escape(curl_, json_object_get_string(jobj), 0);
					param += encodedKey + std::string("=") + encodedVal;
					curl_free(encodedKey);
					curl_free(encodedVal);
				}			
				break;
			case json_type_object :
				ParseJSON(param, key, jobj);
				break;
			case json_type_array :
				{
					json_object *jarray = jobj;
					int arraylen = json_object_array_length(jarray);
					for (int i=0; i< arraylen; i++)
					{ 
						json_object* jvalue = json_object_array_get_idx(jarray, i);
						std::string array_key = key + std::string("[]");
						ParseJSON(param, array_key.c_str(), jvalue);
					}
				}
				break;
			default :
				break;
		}
	}
}
*/

bool HttpClient::Get(const char* path, const char* param, std::function<void(int errcode, const char* data)> callback)
{
	if(NULL == curl_)
	{
		errno = __LINE__;
		return false;
	}

	std::string path_param = path;
	if(NULL != param && 0 < strlen(param))
	{
		path_param +=  "?" + std::string(param);
	}

	int httpCode = 0;
	if(false == HttpRequest(path_param.c_str(), httpCode))
	{
		return false;
	}

	callback(httpCode, resData_.c_str());
	return true;
}

bool HttpClient::Post(const char* path, const char* param, std::function<void(int errcode, const char* data)> callback)
{
	curl_slist *header_list=NULL;
	errno = 0;
	do 
	{
		if(NULL == curl_)
		{
			errno = __LINE__;
			break;
		}

		for(auto itr : mapHeader_)
		{
			std::string header = itr.first + ": " + itr.second;
			header_list = curl_slist_append(header_list, header.c_str());
		}

		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_list))
		{
			errno = __LINE__;
			break;
		}

		int nPostFieldSize = 0;
		if(NULL != param && 0 < strlen(param))
		{
			if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, param))
			{
				errno = __LINE__;
				break;
			}
			nPostFieldSize = strlen(param);
		}

		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, nPostFieldSize))
		{
			errno = __LINE__;
			break;
		}

		int httpCode = 0;
		if(false == HttpRequest(path, httpCode))
		{
			break;
		}

		callback(httpCode, resData_.c_str());
	} while(false);

	if(NULL != header_list)
	{
		curl_slist_free_all(header_list);
		header_list = NULL;
	}

	if(0 != errno)
	{
		return false;
	}
	return true;
}

bool HttpClient::HttpRequest(const char* path, int& httpCode)
{
	if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1))
	{
		errno = __LINE__;
		return false;
	}
	const std::string url = host_ + "/" + std::string(path);
	if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_URL, url.c_str()))
	{
		errno = __LINE__;
		return false;
	}

	std::string protocol = host_.substr(0, host_.find("://"));
	std::transform(protocol.begin(), protocol.end(), protocol.begin(), ::toupper);
	if("HTTPS" == protocol)
	{
		if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 1L))
		{
			errno = __LINE__;
			return false;
		}
	}

	if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)this))
	{
		errno = __LINE__;
		return false;
	}

	if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, Callback))
	{
		errno = __LINE__;
		return false;
	}

	while(true)
	{
		if(CURLE_OK != curl_easy_perform(curl_))
		{
			errno = __LINE__;
			return false;
		}

		long _httpCode = 0;
		if(CURLE_OK != curl_easy_getinfo(curl_, CURLINFO_HTTP_CODE, &_httpCode))
		{
			errno = __LINE__;
			return false;
		}

		httpCode = _httpCode;
		if(3 == httpCode / 100)
		{
			curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
			continue;
		}
		break;
	}
	return true;
}

size_t HttpClient::Callback(void *ptr, size_t size, size_t nmemb, void *arg)
{
	const size_t byte_size_of_ptr = size * nmemb;
	HttpClient* self = (HttpClient*)arg;
	self->resData_ += (const char*)ptr;
	return byte_size_of_ptr;
}

}};
