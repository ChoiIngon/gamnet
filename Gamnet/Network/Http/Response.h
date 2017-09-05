#ifndef GAMNET_NETWORK_HTTP_RESPONSE_H_
#define GAMNET_NETWORK_HTTP_RESPONSE_H_

#include <string>
#include <map>

namespace Gamnet { namespace Network { namespace Http {

class Response {
public:
	Response();
	virtual ~Response();

	int error_code;
	std::string context;
};

const char* GetErrorStr(int error_code);
extern std::map<int, std::string> mapErrorCode;

}}}
#endif /* RESPONSE_H_ */
