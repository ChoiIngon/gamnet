/*
 * Exception.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef __GAMNET_LIB_EXCEPTION_H_
#define __GAMNET_LIB_EXCEPTION_H_

#include "String.h"
namespace Gamnet {

class Exception : public std::exception
{
private :
	const std::string detail_;
	int error_code_;
public :
	Exception(const std::string& s, int error_code = 0) : detail_(s), error_code_(error_code) {}
	virtual ~Exception() throw() {}
	virtual const char* what() const throw()
	{
		return detail_.c_str();
	}
	int error_code() const throw()
	{
		return error_code_;
	}
};

}

#define GAMNET_EXCEPTION(detail, error_code) \
	Gamnet::Exception(Gamnet::Format("exception at ", __FILE__, ":", __func__, "@" , __LINE__, ", what:", detail), error_code)


#endif /* EXCEPTION_H_ */
