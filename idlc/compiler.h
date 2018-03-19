#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <string>
#include <map>

#include "token.h"

class Compiler 
{
	std::string 	m_sFileName;
	std::string 	m_sOption;
public :
	Compiler(const std::string& sFileName, const std::string& sOption);
	~Compiler();

	bool Compile();
};

#endif //__COMPILER_H__
