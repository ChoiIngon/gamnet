#include "Config.h"

#include <boost/property_tree/xml_parser.hpp>

#include "../Library/Exception.h"

namespace Gamnet { namespace Test {
	void Config::ReadXml(const std::string& config)
	{
		boost::property_tree::ptree ptree_;
		try {
			boost::property_tree::xml_parser::read_xml(config, ptree_);
		}
		catch (const boost::property_tree::xml_parser_error& e)
		{
			throw GAMNET_EXCEPTION(ErrorCode::FileNotFound, "config file not found error(path:", config, ")");
		}

		try {
			auto elmtConditions = ptree_.get_child("server.test");
			for (const auto& elmtCondition : elmtConditions)
			{
				Condition condition;
				condition.name = elmtCondition.first;
				if ("<xmlattr>" == condition.name)
				{
					continue;
				}
				auto elmtCommands = elmtCondition.second.get_child("");
				condition.parameters = ReadAttribute(elmtCondition.second);
				for (const auto& elmtCommand : elmtCommands)
				{					
					Command command;
					command.name = elmtCommand.first;
					if("<xmlattr>" == command.name)
					{
						continue;
					}

					command.parameters = ReadAttribute(elmtCommand.second);
					condition.commands.push_back(command);
				}
				conditions[MakeKey(condition.name, condition.parameters)] = condition;
			}
		}
		catch (const boost::property_tree::ptree_bad_path& e)
		{
			throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, e.what());
		}
	}

	const Config::Condition& Config::OnCondition(const std::string& name, const std::map<std::string, boost::any>& parameter)
	{
		auto itr = conditions.find(MakeKey(name, parameter));
		if(conditions.end() == itr)
		{
			throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "can not find condition(name:", name, ")");
		}
		return itr->second;
	}
	std::map<std::string, boost::any> Config::ReadAttribute(const boost::property_tree::ptree& element)
	{
		std::map<std::string, boost::any> parameters;
		boost::optional<const boost::property_tree::ptree&> child = element.get_child_optional("<xmlattr>");
		if(false == (bool)child)
		{
			return parameters;
		}
		const auto& attributes = element.get_child("<xmlattr>");
		for (const auto& attribute : attributes)
		{
			parameters[attribute.first] = attribute.second.get_value<std::string>();
		}
		return parameters;
	}

	std::string Config::MakeKey(const std::string& name, const std::map<std::string, boost::any>& parameter)
	{
		std::string key = name + ";";
		for(auto& itr : parameter)
		{
			key += itr.first + "=" + boost::any_cast<std::string>(itr.second) + ";";
		}
		return key;
	}
}}