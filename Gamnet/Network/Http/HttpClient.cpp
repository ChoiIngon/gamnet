#include <curl/curl.h>
#include <openssl/crypto.h>
#include <algorithm>
#include <map>
#include <string>
#include <cstring>
#include <vector>

#include "../../Log/Log.h"
struct curl_slist;

#include "HttpClient.h"
//#include <unistd.h>

#define USE_OPENSSL

namespace Gamnet { namespace Network { namespace Http {

/**
 * code copied from : http://curl.haxx.se/libcurl/c/threaded-ssl.html
 */
#ifdef _WIN32
static std::vector<std::mutex> lockarray_(CRYPTO_num_locks());
static void lock_callback(int mode, int type, const char *file, int line)
{
	(void)file;
	(void)line;
	if (mode & CRYPTO_LOCK) {
		lockarray_[type].lock();
	}
	else {
		lockarray_[type].unlock();
	}
}

static unsigned long thread_id(void)
{
	unsigned long ret = (unsigned long)GetCurrentThreadId();
	return ret ;
}

static void init_locks(void)
{
	//lockarray_.resize(CRYPTO_num_locks());
	CRYPTO_set_id_callback((unsigned long (*)())thread_id);
	CRYPTO_set_locking_callback(lock_callback);
}
#else
static pthread_mutex_t* lockarray_;

static void lock_callback(int mode, int type, const char *file, int line)
{
	(void)file;
	(void)line;
	if (mode & CRYPTO_LOCK) {
		pthread_mutex_lock(&(lockarray_[type]));
	}
	else {
		pthread_mutex_unlock(&(lockarray_[type]));
	}
}

static unsigned long thread_id(void)
{
	unsigned long ret = (unsigned long)pthread_self();
	return ret ;
}

static void init_locks(void)
{
	lockarray_ = (pthread_mutex_t *)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
	for (int i=0; i<CRYPTO_num_locks(); i++) {
		pthread_mutex_init(&(lockarray_[i]),NULL);
	}
	CRYPTO_set_id_callback((unsigned long (*)())thread_id);
	CRYPTO_set_locking_callback(lock_callback);
}

static void kill_locks(void)
{
	CRYPTO_set_locking_callback(NULL);
	for (int i=0; i<CRYPTO_num_locks(); i++) {
		pthread_mutex_destroy(&(lockarray_[i]));
	}
	OPENSSL_free(lockarray_);
}
#endif
static CURLcode HttpClientInit()
{
	CURLcode globalCURL = curl_global_init(CURL_GLOBAL_ALL);
	init_locks();
	return globalCURL;
}

static CURLcode globalCURL = HttpClientInit();

HttpClient::HttpClient(const char* host) 
	: host_(host)
{
	curl_ = curl_easy_init();
	int error_code = 0;
	error_code = curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, false);
	if (CURLE_OK != error_code)
	{
		throw GAMNET_EXCEPTION(500, "set ssl option error(error_code:", error_code, ")");
	}

	error_code = curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, false);
	if (CURLE_OK != error_code)
	{
		throw GAMNET_EXCEPTION(500, "set ssl option error(error_code:", error_code, ")");
	}
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

	std::string path_param = "";
	if(NULL != path && 0 < strlen(path))
	{
		path_param = path;
	}

	if(NULL != param && 0 < strlen(param))
	{
		path_param +=  "?" + std::string(param);
	}

	int httpCode = 200;
	try {
		httpCode = HttpRequest(path_param);
		callback(httpCode, resData_.c_str());
	}
	catch(const Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		return false;
	}
	
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

		std::string path_param = "";
		if(NULL != path && 0 < strlen(path))
		{
			path_param = path;
		}
		httpCode = HttpRequest(path_param);
		callback(httpCode, resData_.c_str());
	}
	catch(const Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		return false;
	}
	
	return true;
}

bool HttpClient::Put(const char* path, const char* param, std::function<void(int stat, const char* data)> callback)
{
	if(NULL == curl_)
	{
		return false; //Exception(500, "curl lib isn't inited");
	}

	int httpCode = 200;
	try {
		if(NULL != param && 0 < strlen(param))
		{
			if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, param))
			{
				throw Exception(500, "set post field error");
			}
		}
		curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */

		std::string path_param = "";
		if(NULL != path && 0 < strlen(path))
		{
			path_param = path;
		}
		httpCode = HttpRequest(path_param);
		callback(httpCode, resData_.c_str());
	}
	catch(const Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		return false;
	}
	
	return true;
}

bool HttpClient::Delete(const char* path, const char* param, std::function<void(int stat, const char* data)> callback)
{
	if(NULL == curl_)
	{
		return false; //Exception(500, "curl lib isn't inited");
	}

	int httpCode = 200;
	try {
		if(NULL != param && 0 < strlen(param))
		{
			if(CURLE_OK != curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, param))
			{
				throw Exception(500, "set post field error");
			}
		}
		curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE"); /* !!! */

		std::string path_param = "";
		if(NULL != path && 0 < strlen(path))
		{
			path_param = path;
		}
		httpCode = HttpRequest(path_param);
		callback(httpCode, resData_.c_str());
	}
	catch(const Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		return false;
	}
	
	return true;
}

int HttpClient::HttpRequest(const std::string& path)
{
	curl_slist *header_list=NULL;
	long _httpCode = 200;

	try {
		if(NULL == curl_)
		{
			throw GAMNET_EXCEPTION(500, "curl lib isn't inited");
		}

		for(const auto& itr : mapHeader_)
		{
			std::string header = itr.first + ": " + itr.second;
			header_list = curl_slist_append(header_list, header.c_str());
		}

		CURLcode error_code = CURLE_OK;
		
		error_code = curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_list);
		if(CURLE_OK != error_code)
		{
			throw GAMNET_EXCEPTION(500, "set header error(error_code:", error_code,")");
		}

		error_code = curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 5);
		if(CURLE_OK != error_code)
		{
			throw GAMNET_EXCEPTION(500, "set connect timeout option error(error_code:", error_code,")");
		}
		error_code = curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1);
		if(CURLE_OK != error_code)
		{
			throw GAMNET_EXCEPTION(500, "set no signal option error(error_code:", error_code,")");
		}

		std::string url = host_;
		if("" != path)
		{
			url += "/" + path;
		}
		error_code = curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
		if(CURLE_OK != error_code)
		{
			throw GAMNET_EXCEPTION(500, "set url error(error_code:", error_code, ")");
		}

		error_code = curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)this);
		if(CURLE_OK != error_code)
		{
			throw GAMNET_EXCEPTION(500, "set callback arg error(error_code:", error_code, ")");
		}

		error_code = curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, Callback);
		if(CURLE_OK != error_code)
		{
			throw GAMNET_EXCEPTION(500, "set callback function error(error_code:", error_code, ")");
		}

		// int errorCount = 0;
		for(int i=0; i<10; i++)
		{
			error_code = curl_easy_perform(curl_);
			if(CURLE_OK != error_code)
			{
				throw GAMNET_EXCEPTION(500, "perform http request error(error_code:", error_code, ", try_count : ", 0+1, ", url : ", url, ")");
				//LOG(ERR,"perform http request error(error_code:", error_code, ", try_count:", 0+1, ", url:", url, ")");
				//continue;
			}

			error_code = curl_easy_getinfo(curl_, CURLINFO_HTTP_CODE, &_httpCode);
			if(CURLE_OK != error_code)
			{
				throw GAMNET_EXCEPTION(500, "get return error code error(error_code:", error_code, ")");
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
	self->resData_ += std::string((const char*)ptr, byte_size_of_ptr); ;
	return byte_size_of_ptr;
}

}}};
