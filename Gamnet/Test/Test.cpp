#include "Test.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace Gamnet { namespace Test {
	static std::function<void()> _runFunctor = [](){};
	
	void Run(int thread_count)
	{
		_runFunctor();
	}

	void RegisterRun(std::function<void()> runFunctor)
	{
		_runFunctor = runFunctor;
	}

	void Config::ReadXml(const std::string& path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(path, ptree_);

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
}}
