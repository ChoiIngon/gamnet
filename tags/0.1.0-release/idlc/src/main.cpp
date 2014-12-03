#include <iostream>
#include <string>
#include "compiler.h"
#include "parser.h"
#include "generate_rule_cpp.h"
#include "generate_rule_python.h"

void DisplayUsage()
{
	std::cout << "idlc -l [cpp|cs|py] <input_file>" << std::endl;
}

std::string sFileName = "";
std::string sLanguage = "";

bool GetOption(int argc, char* argv[])
{
	if(3 > argc)
	{
		return false;
	}

	for(int i=1; i<argc; i++)
	{
		std::string sOption = argv[i];

		if("" != sLanguage)
		{
			sFileName = argv[i];
			if(std::string::npos != sFileName.find(".idl"))
			{
				return true;
			}
			DisplayUsage();
			return false;
		}
		if(std::string::npos != sOption.find("-l"))
		{
			sLanguage = sOption.substr(sOption.find("-l") + std::string("-l").length());
			if("" == sLanguage)
			{
				i++;
				if(argc <= i)
				{
					std::cout << "\"-l\" option error" << std::endl;
					return false;
				}

				sLanguage = argv[i];
			}
		}
	}
	return false;
}

int main(int argc, char* argv[])
{
	if(false == GetOption(argc, argv))
	{
		DisplayUsage();
	}

	Compiler* pComp = new Compiler(sFileName, sLanguage);
	pComp->Compile();

	return 0;
}
