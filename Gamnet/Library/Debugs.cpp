#include "Debugs.h"

#ifdef _WIN32
#include <Dbghelp.h>
#include <stdio.h>
#pragma comment(lib, "Dbghelp.lib")
#endif
namespace Gamnet {
#ifdef _WIN32
LONG WINAPI UnhandledException(PEXCEPTION_POINTERS exception_pointers)
{
	char dump_file_name[256];
	SYSTEMTIME systemtime;
	GetSystemTime(&systemtime);
	sprintf_s(dump_file_name, "%04d-%02d-%02d_%02d-%02d-%02d_%04x_%04x.dmp",
		systemtime.wYear, systemtime.wMonth, systemtime.wDay, systemtime.wHour, systemtime.wMinute, systemtime.wSecond, GetCurrentProcessId(), GetCurrentThreadId());
	HANDLE file_handle = CreateFileA(dump_file_name, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(file_handle == INVALID_HANDLE_VALUE)
		return EXCEPTION_EXECUTE_HANDLER;

	MINIDUMP_EXCEPTION_INFORMATION minidump_exception_information;
	minidump_exception_information.ThreadId = GetCurrentThreadId();
	minidump_exception_information.ExceptionPointers = exception_pointers;
	minidump_exception_information.ClientPointers = FALSE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle, /*normal_dump ? MiniDumpNormal :*/
			static_cast<MINIDUMP_TYPE>(
				MiniDumpWithFullMemory | 
				MiniDumpWithHandleData | 
				MiniDumpWithProcessThreadData |
				MiniDumpWithFullMemoryInfo | 
				MiniDumpWithThreadInfo
			), &minidump_exception_information, nullptr, nullptr);

	CloseHandle(file_handle);

	return EXCEPTION_EXECUTE_HANDLER;
}

#endif
}