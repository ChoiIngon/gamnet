#include "Test.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace Gamnet { namespace Test {
	Config::Config()
		: port(0)
		, session_count(0)
		, loop_count(0)
	{
	}

	
	void Config::ReadXml(const std::string& path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(path, ptree_);

		try {
			auto optional = ptree_.get_child_optional("server.test");
			if(false == (bool)optional)
			{
				return;
			}
			host = ptree_.get<std::string>("server.test.<xmlattr>.host");
			port = ptree_.get<int>("server.test.<xmlattr>.port");
			session_count = ptree_.get<int>("server.test.<xmlattr>.session_count");
			loop_count = ptree_.get<int>("server.test.<xmlattr>.loop_count");
			auto test_case = ptree_.get_child("server.test");
			for (const auto& elmt : test_case)
			{
				if ("message" == elmt.first)
				{
					messages.push_back(elmt.second.get<std::string>("<xmlattr>.name"));
				}
			}
		}
		catch (const boost::property_tree::ptree_bad_path& e)
		{
			std::cerr << "[Gamnet::Test] " << e.what() << std::endl;
			throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, e.what());
		}
	}
}}
