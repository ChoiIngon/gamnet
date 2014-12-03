/*
 * Exception.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef __GAMNET_LIB_EXCEPTION_H_
#define __GAMNET_LIB_EXCEPTION_H_

#include "String.h"
#include "ErrorCode.h"
namespace Gamnet {

class Exception : public std::exception
{
	int error_code_;
	const std::string detail_;
public :
	template <class... ARGS>
	Exception(int error_code, ARGS... args) : error_code_(error_code), detail_(Format(args...))
	{
	}
	virtual ~Exception() throw();
	virtual const char* what() const throw();
	int error_code() const throw();
};

}

#define GAMNET_ERRNO(errno) errno, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] ", #errno
#endif /* EXCEPTION_H_ */
