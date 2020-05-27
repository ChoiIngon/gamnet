/*
 * Request.h
 *
 *  Created on: 2017. 8. 26.
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HTTP_REQUEST_H_
#define GAMNET_NETWORK_HTTP_REQUEST_H_

#include "../../Library/Variant.h"
#include "../../Library/Exception.h"
#include <map>

namespace Gamnet {
namespace Network {
namespace Http {

class Request {
	std::map<std::string, Variant> params;
public:
	Request(const std::string& param);
	virtual ~Request();

	const Variant& operator[](const std::string& name) const;
};

} /* namespace Http */
} /* namespace Network */
} /* namespace Gamnet */

#endif /* NETWORK_HTTP_REQUEST_H_ */
