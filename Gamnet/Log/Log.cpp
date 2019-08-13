/*
 * Log.cpp
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#include "Logger.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Log {

void Init(const char* log_dir, const char* prefix, int max_file_size)
{
	Logger::GetInstance().Init(log_dir, prefix, max_file_size);
}

void ReadXml(const char* xml_path)
{
	boost::property_tree::ptree ptree_;
	boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

	std::string path = ptree_.get<std::string>("server.log.<xmlattr>.path");
	std::string prefix = ptree_.get<std::string>("server.log.<xmlattr>.prefix");
	int max_size = ptree_.get<int>("server.log.<xmlattr>.max_file_size");
	Init(path.c_str(), prefix.c_str(), max_size);
	auto log = ptree_.get_child("server.log");
	for(const auto& elmt : log)
	{
		Logger::LOG_LEVEL_TYPE level = Logger::LOG_LEVEL_MIN;
		if("dev" == elmt.first)
		{
			level = Logger::LOG_LEVEL_DEV;
		}
		if("inf" == elmt.first)
		{
			level = Logger::LOG_LEVEL_INF;
		}
		if("err" == elmt.first)
		{
			level = Logger::LOG_LEVEL_ERR;
		}
		if("wrn" == elmt.first)
		{
			level = Logger::LOG_LEVEL_WRN;
		}
		if(Logger::LOG_LEVEL_MIN == level)
		{
			continue;
		}

		int flag = 0;
		if("yes" == elmt.second.get<std::string>("<xmlattr>.console"))
		{
			flag |= Logger::LOG_STDERR;
		}
		if("yes" == elmt.second.get<std::string>("<xmlattr>.file"))
		{
			flag |= Logger::LOG_FILE;
		}
		if("yes" == elmt.second.get<std::string>("<xmlattr>.sys"))
		{
			flag |= Logger::LOG_SYSLOG;
		}

		Logger::GetInstance().SetLevelProperty(level, flag);
	}
}

void SetLevelProperty(Logger::LOG_LEVEL_TYPE level, int flag)
{
	Logger::GetInstance().SetLevelProperty(level, flag);
}

}}


