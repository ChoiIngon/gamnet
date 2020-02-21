/*
 * Exception.cpp
 *
 *  Created on: Aug 29, 2014
 *      Author: kukuta
 */
#include "Exception.h"

namespace Gamnet
{
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
				ss << Format(line.FileName, "!", symbol->Name, "@", line.LineNumber);
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
