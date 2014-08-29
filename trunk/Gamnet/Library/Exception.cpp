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
}


