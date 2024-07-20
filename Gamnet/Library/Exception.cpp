module;

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#include <Winbase.h>
#endif

export module Gamnet.Exception;

import Gamnet.String;
import <exception>;
import <string>;
import <sstream>;

namespace Gamnet
{
	export enum ErrorCode {
		
		Success = 0,
		ConnectFailError = 1,
		ReconnectFailError = 2,
		HeartbeatFailError = 3,
		SendMsgFailError = 4,
		ConnectTimeoutError = 10,
		ResponseTimeoutError = 11,
		InvalidArgumentError = 20,
		InvalidSessionTokenError = 21,
		InvalidSessionError = 22,
		InvalidSessionKeyError = 23,
		InvalidSessionStateError = 24,
		InvalidArrayRangeError = 30,
		InvalidKeyError = 40,
		InvalidAddressError = 41,
		InvalidLinkManagerError = 42,
		InvalidHandlerError = 43,
		InvalidDatabaseNum = 44,
		InvalidLinkError = 45,
		InvalidDateTimeFormat = 46,
		InvalidSocketError = 47,
		NotInitializedError = 50,
		NullPointerError = 60,
		NullPacketError = 61,
		MessageFormatError = 70,
		DuplicateConnectionError = 80,
		CreateDirectoryFailError = 90,
		CreateInstanceFailError = 91,
		BufferOverflowError = 100,
		BufferUnderflowError = 101,
		MessageSeqOmittedError = 110,
		BadLexicalCastError = 120,
		AcceptFailError = 121,
		SendQueueOverflowError = 123,
		IdleTimeoutError = 130,
		DuplicateMessageIDError = 140,
		DuplicateRouterAddress = 141,
		SetRouterAddressError = 142,
		RouterCastTypeErrror = 143,
		InvalidRouterAddress = 144,
		AlreadyCommitTransaction = 150,
		AlreadyRegisteredAddress = 151,
		CipherContextCreateFail = 200,
		CipherContextInitFail = 201,
		EncryptUpdateFail = 202,
		EncryptFinalFail = 203,
		DecryptUpdateFail = 204,
		DecryptFinalFail = 205,
		RedisResultError = 300,
		ExecuteQueryError = 301,
		FileNotFound = 400,
		SystemInitializeError = 401,
		UndefinedError = 999
	};

	export class Exception : public std::exception
	{
		int error_code_;
		const std::string detail_;
	public:
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
	
	Exception::Exception(int error_code) : error_code_(error_code)
	{
	}
	
	Exception::~Exception() throw() {}
	
	const char* Exception::what() const throw()
	{
		return detail_.c_str();
	}
	
	int Exception::error_code() const throw()
	{
		return error_code_;
	}
	
	const char* Exception::stack_trace()
	{
		/*
		void* trace[16];
		int trace_size = backtrace(trace, 16);
		char** messages = backtrace_symbols(trace, trace_size);
		const_cast<std::string&>(detail_) += "\n";
		for(int i=1; i<trace_size-2; i++)
		{
			int p = 0;
			while('(' != messages[i][p] && ' ' != messages[i][p] && 0 != messages[i][p])
			{
				++p;
			}
			char syscom[256];
			sprintf(syscom,"addr2line %p -e %.*s", trace[i] , p, messages[i] );
			//last parameter is the filename of the symbol
			char buff[1024] = {0,};
			FILE* fp = popen(syscom, "r");
			while(NULL != fgets(buff, 1024, fp))
			{
				const_cast<std::string&>(detail_) += Format("\t#", i, ": ", buff);
			}
			pclose(fp);
		}
		*/
		return nullptr;
	}

#ifdef _WIN32
#pragma comment(lib, "Dbghelp.lib")
	export class StackTrace
	{
	public:
		class SymbolHandler
		{
		private:
			bool initialized;
		public:
			SymbolHandler();
			std::string GetSymbolInfo(DWORD64 addr);
			void CaptureStackTrace(CONTEXT* context, DWORD64* frame_pointers, size_t count, size_t skip);
		};
	private:
		static SymbolHandler symbol_handler;
		static const size_t MAX_FRAME_POINTERS = 256;
		DWORD64 frame_ptrs[MAX_FRAME_POINTERS];
	public:
		StackTrace(CONTEXT* context = nullptr, size_t skip = 0);
		std::string to_string() const;
	};
	
	StackTrace::SymbolHandler::SymbolHandler()
	{
		initialized = TRUE == SymInitialize(GetCurrentProcess(), nullptr, TRUE);
	}
	
	std::string StackTrace::SymbolHandler::GetSymbolInfo(DWORD64 addr)
	{
		std::stringstream ss;
		DWORD64 displacement64;
		DWORD displacement;
		const int buffer_size = 1024;
		char symbol_buffer[sizeof(SYMBOL_INFO) + buffer_size - 1];
		SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbol_buffer);
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = buffer_size;

		IMAGEHLP_LINE64 line;
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		//ss << boost::format("[0x%08X] ") % addr;  

		if (true == initialized)
		{
			if (SymFromAddr(GetCurrentProcess(), addr, &displacement64, symbol))
			{
				if (SymGetLineFromAddr64(GetCurrentProcess(), addr, &displacement, &line))
				{
					//ss << (boost::format(" (%s:%d)") % line.FileName % line.LineNumber).str();  						
					ss << line.FileName << "!" << symbol->Name << "@" << line.LineNumber;
				}
			}
			else {
				DWORD err = GetLastError();
			}
		}
		return ss.str();
	}

	void StackTrace::SymbolHandler::CaptureStackTrace(CONTEXT* context, DWORD64* frame_pointers, size_t count, size_t skip)
	{
		if (true == initialized)
		{
			CONTEXT current_context;
			if (nullptr == context)
			{
				RtlCaptureContext(&current_context);
				context = &current_context;
			}

			DWORD machine_type;
			STACKFRAME64 frame;
			ZeroMemory(&frame, sizeof(frame));
			frame.AddrPC.Mode = AddrModeFlat;
			frame.AddrFrame.Mode = AddrModeFlat;
			frame.AddrStack.Mode = AddrModeFlat;
	#ifdef _M_X64  
			frame.AddrPC.Offset = context->Rip;
			frame.AddrFrame.Offset = context->Rbp;
			frame.AddrStack.Offset = context->Rsp;
			machine_type = IMAGE_FILE_MACHINE_AMD64;
	#else  
			frame.AddrPC.Offset = context->Eip;
			frame.AddrPC.Offset = context->Ebp;
			frame.AddrPC.Offset = context->Esp;
			machine_type = IMAGE_FILE_MACHINE_I386;
	#endif  
			for (size_t i = 0; i < count + skip; i++)
			{
				if (TRUE == StackWalk64(machine_type,
					GetCurrentProcess(),
					GetCurrentThread(),
					&frame,
					context,
					NULL,
					SymFunctionTableAccess64,
					SymGetModuleBase64,
					NULL))
				{
					if (i >= skip)
					{
						frame_pointers[i - skip] = frame.AddrPC.Offset;
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	StackTrace::SymbolHandler StackTrace::symbol_handler = StackTrace::SymbolHandler();

	StackTrace::StackTrace(CONTEXT* context, size_t skip)
	{
		::ZeroMemory(frame_ptrs, sizeof(frame_ptrs));
		symbol_handler.CaptureStackTrace(context, frame_ptrs, MAX_FRAME_POINTERS, skip);
	}

	std::string StackTrace::to_string() const
	{
		std::stringstream ss;
		for (size_t i = 0; i < MAX_FRAME_POINTERS && 0 != frame_ptrs[i]; i++)
		{
			std::string stack = symbol_handler.GetSymbolInfo(frame_ptrs[i]);
			if("" == stack)
			{
				continue;
			}
			ss << i+1 << " : " << stack << "\n";
		}
		return ss.str();
	}
#endif
}
