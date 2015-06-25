#ifndef __GAMNET_HTTP_HTTP_CLIENT_H__
#define __GAMNET_HTTP_HTTP_CLIENT_H__

/*!
 * \brief HttpClient
 *
 * 'http' protocol client library - using CURL(http://curl.haxx.se/libcurl/c/example.html)<br>
 * support sync call only.<br>
 * support 'get' and 'post' both method<br>
 * support 'JSON' format as parameters<br>
 *
 * Sample code :<br>
 * <pre>
 * 	int main() {
 * 		Toolkit::HttpClient post_client("http://localhost");
 * 		post_client.SetHeader("Content-Type", "application/x-www-form-urlencoded");
 * 		post_client.Post("index.php",
 * 					"{ \
 * 						'accountType':'HOSTED_OR_GOOGLE',\
 * 						'Email':'gamevil.push@gmail.com',\
 * 						'Passwd':'rpdlaqlf123',\
 * 						'service':'ac2dm',\
 * 						'source':'gamevil-c2dm-1.0',\
 * 						'array_example':[\
 * 							'array_elmt_0',\
 *	 						'array_elmt_1',\
 * 							'array_elmt_2',\
 * 							'array_elmt_3'\
 * 						]\
 * 					}",
 * 					[](int stat, const char* data) {
 * 						std::cout << stat << std::endl;
 * 						std::cout << data << std::endl;
 * 					}
 * 			);
 *
 * 		}
 *
 * 		HttpClient get_client("http://comic.naver.com");
 * 		get_client.Get("webtoon/list.nhn",
 * 					"{ \
 * 						'titleId':574303,\
 * 						'weekday':'thu'\
 * 					}",
 * 					[](int stat, const char* data) {
 * 						std::cout << stat << std::endl;
 * 						std::cout << data << std::endl;
 * 					}
 * 			);
 * 		return 0;
 * 	}
 *  </pre>
 */
#include <curl/curl.h>
#include <map>
#include <string>
#include <iostream>
#include <functional>
#include <algorithm>

namespace Gamnet {
namespace Http {

class HttpClient
{
public :
	HttpClient(const char* host);
	virtual ~HttpClient();

	void SetHeader(const char* name, const char* value)
	{
		mapHeader_[name] = value;
	}

	void UseSSLVerifier(bool use);
	bool Post(const char* path, const char* param, std::function<void(int stat, const char* data)> callback);
	bool Get(const char* path, const char* param, std::function<void(int stat, const char* data)> callback);
	bool Put(const char* path, const char* param, std::function<void(int stat, const char* data)> callback);
private :
	int HttpRequest(const std::string& path);
	static size_t Callback(void *ptr, size_t size, size_t nmemb, void *arg);
	const std::string host_;
	std::map<std::string, std::string> mapHeader_;
	std::string resData_;
	CURL* curl_;
};

}};
#endif // __HTTP_H__
