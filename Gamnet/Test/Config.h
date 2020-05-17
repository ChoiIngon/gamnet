#ifndef _GAMNET_TEST_CONFIG_H_
#define _GAMNET_TEST_CONFIG_H_

#include <boost/property_tree/ptree.hpp>
#include <list>
#include <map>
#include <string>
#include "../Library/Variant.h"

namespace Gamnet { namespace Test {
	class Config
	{
		struct Command
		{
			std::string name;
			std::map<std::string, Variant> parameters;
		};

		struct Condition
		{
			std::string name;
			std::map<std::string, Variant> parameters;
			std::list<Command> commands;
		};
		
		std::map<std::string, Condition> conditions;
	public :
		void ReadXml(const std::string& config);
		const Condition& OnCondition(const std::string& name, const std::map<std::string, Variant>& parameter);
	private :
		std::map<std::string, Variant> ReadAttribute(const boost::property_tree::ptree& element);
		std::string MakeKey(const std::string& name, const std::map<std::string, Variant>& parameter);
	};
}}
#endif
