/*
 * Response.h
 *
 *  Created on: Aug 11, 2014
 *      Author: kukuta
 */

#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <string>
#include <map>

namespace Gamnet {
namespace Http {

class Response {
public:
	Response();
	virtual ~Response();

	int nErrorCode;
	std::string sBodyContext;
};

const char* GetErrorStr(int error_code);
extern std::map<int, std::string> mapErrorCode;
} /* namespace Http */
} /* namespace Gamnet */
#endif /* RESPONSE_H_ */
