/*
 * Request.cpp
 *
 *  Created on: 2017. 8. 26.
 *      Author: kukuta
 */

#include "Request.h"

namespace Gamnet {
namespace Network {
namespace Http {

Request::Request(const std::string& param_string) {
	std::string param = param_string;
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
		params.insert(std::make_pair(name, value));
		if(delim_pos >= param.length())
		{
			break;
		}
		param = param.substr(delim_pos+1);
	}
}

Request::~Request() {
	// TODO Auto-generated destructor stub
}

const Variant& Request::operator[](const std::string& name) const
{
	auto itr = params.find(name);
	if(params.end() == itr)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::InvalidKeyError), "(key:", name, ")");
	}

	return itr->second;
}
} /* namespace Http */
} /* namespace Network */
} /* namespace Gamnet */
