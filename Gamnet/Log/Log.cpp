/*
 * Log.cpp
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#include "Logger.h"
#include "../Library/Singleton.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Log {

void Init(const char* log_dir, int max_file_size)
{
	Singleton<Logger>().Init(log_dir, max_file_size);
}

void ReadXml(const char* xml_path)
{
	try
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		std::string path = ptree_.get<std::string>("server.log.<xmlattr>.path");
		int max_size = ptree_.get<int>("server.log.<xmlattr>.max_file_size");
		Init(path.c_str(), max_size);
		auto log = ptree_.get_child("server.log");
		for(auto elmt : log)
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

			Singleton<Logger>().SetLevelProperty(level, flag);
		}
	}
	catch (boost::exception& e)
	{
		std::cerr << boost::diagnostic_information(e) << "(file_path:" << xml_path << ")" << std::endl;
		return;
	}

}

void SetLevelProperty(Logger::LOG_LEVEL_TYPE level, int flag)
{
	Singleton<Logger>().SetLevelProperty(level, flag);
}

}}


