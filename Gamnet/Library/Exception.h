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

class ErrorCode { 
public :
	enum {
		Success						= 0,
		ConnectFailError			= 1,
		AcceptFailError				= 2,
		ConnectTimeoutError			= 10,
		InvalidArgumentError		= 20,
		InvalidSessionTokenError	= 21,
		InvalidSessionError			= 22,
		InvalidArrayRangeError		= 30,
		InvalidKeyError				= 40,
		InvalidAddressError			= 41,
		InvalidLinkManagerError		= 42,
		InvalidHandlerError			= 43,
		NotInitializedError			= 50,
		NullPointerError			= 60,
		NullPacketError				= 61,
		MessageFormatError			= 70,
		DuplicateConnectionError	= 80,
		CreateDirectoryFailError	= 90,
		CreateInstanceFailError		= 91,
		BufferOverflowError			= 100,
		BufferUnderflowError		= 101,
		MessageSeqOmittedError		= 110,
		BadLexicalCastError			= 120,
		IdleTimeoutError			= 130,
		UndefinedError				= 999
	};
};

class Exception : public std::exception
{
	int error_code_;
	const std::string detail_;
public :
	template <class... ARGS>
	Exception(int error_code, ARGS... args) : error_code_(error_code), detail_(Format(args...))
	{
		//stack_trace();
	}
	virtual ~Exception() throw();
	virtual const char* what() const throw();
	int error_code() const throw();

private :
	//void stack_trace();
};

}

#define GAMNET_ERRNO(error) (int)error, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] ", #error, " "
#define GAMNET_ERRSTR(error) #error, "(", (int)error, ") "

#endif /* EXCEPTION_H_ */
