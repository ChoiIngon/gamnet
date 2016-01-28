/*
 * Exception.cpp
 *
 *  Created on: Aug 29, 2014
 *      Author: kukuta
 */
#include "Exception.h"
#include <execinfo.h>

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

void Exception::stack_trace()
{
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
}
}
