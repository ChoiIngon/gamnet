#ifndef __GAMNET_LIB_EXCEPTION_H_
#define __GAMNET_LIB_EXCEPTION_H_

#include "String.h"
#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#include <Winbase.h>
#endif
#undef min

namespace Gamnet {

class ErrorCode { 
public :
	enum {
		Success = 0,
		ConnectFailError			= 1,
		ReconnectFailError			= 2,
		HeartbeatFailError			= 3,
		SendMsgFailError			= 4,
		ConnectTimeoutError			= 10,
		ResponseTimeoutError		= 11,
		InvalidArgumentError		= 20,
		InvalidSessionTokenError	= 21,
		InvalidSessionError			= 22,
		InvalidSessionKeyError		= 23,
		InvalidSessionStateError	= 24,
		InvalidArrayRangeError		= 30,
		InvalidKeyError				= 40,
		InvalidAddressError			= 41,
		InvalidLinkManagerError		= 42,
		InvalidHandlerError			= 43,
		InvalidDatabaseNum			= 44,
		InvalidLinkError			= 45,
		InvalidDateTimeFormat		= 46,	
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
		AcceptFailError				= 121,
		SendQueueOverflowError		= 123,
		IdleTimeoutError			= 130,
		DuplicateMessageIDError		= 140,
		DuplicateRouterAddress		= 141,
		SetRouterAddressError		= 142,
		RouterCastTypeErrror		= 143,
		InvalidRouterAddress		= 144,
		AlreadyCommitTransaction	= 150,
		AlreadyRegisteredAddress	= 151,
		CipherContextCreateFail		= 200,
		CipherContextInitFail		= 201,
		EncryptUpdateFail			= 202,
		EncryptFinalFail			= 203,
		DecryptUpdateFail			= 204,
		DecryptFinalFail			= 205,
		RedisResultError			= 300,
		ExecuteQueryError			= 301,
		FileNotFound				= 400,		
		SystemInitializeError		= 401,
		UndefinedError				= 999
	};
};

class Exception : public std::exception
{
	int error_code_;
	const std::string detail_;
public :
	Exception(int error_code);

	template <class... ARGS>
	Exception(int error_code, ARGS... args) : error_code_(error_code), detail_(Format(args...))
	{
		//stack_trace();
	}
	virtual ~Exception() throw();
	virtual const char* what() const throw();
	int error_code() const throw();
	const char* stack_trace();
};

#ifdef _WIN32
#pragma comment(lib, "Dbghelp.lib")

class StackTrace
{
public :
	class SymbolHandler
	{
	private :
		bool initialized;
	public :
		SymbolHandler();
		std::string GetSymbolInfo(DWORD64 addr);
		void CaptureStackTrace(CONTEXT* context, DWORD64* frame_pointers, size_t count, size_t skip);
	};
private:
	static SymbolHandler symbol_handler;
	static const size_t MAX_FRAME_POINTERS = 256;
	DWORD64 frame_ptrs[MAX_FRAME_POINTERS];
public :
	StackTrace(CONTEXT* context = nullptr, size_t skip = 0);
	std::string to_string() const;
};
#endif

}
#define GAMNET_EXCEPTION(error, ...) Gamnet::Exception((int)error, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] ", #error, "(", (int)error,") ", ##__VA_ARGS__)

#endif /* EXCEPTION_H_ */
