#include "example.h"
#include <iostream>

std::string ReplaceSpecialChar(const std::string& ori)
{       
    std::string ret = ori;
    size_t pos = ret.find("[");
	if(pos < ret.length())
	{ 
		ret[pos] = '_';
	}
	pos = ret.find("]");
	if(pos < ret.length())
	{ 
		ret[pos] = '_';
	}
	
	return ret;
}  

int main()
{
	std::cout << ReplaceSpecialChar("1234[i]") << std::endl;;
	return 0;
}
