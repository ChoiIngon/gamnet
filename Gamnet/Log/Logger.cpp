/*
 * Log.cpp
 *
 *  Created on: 2012. 8. 17.
 *      Author: jjaehuny
 */

#include "File.h"
#include "Logger.h"
#include <boost/filesystem.hpp>
#include "../Library/Exception.h"
namespace Gamnet { namespace Log {

void Logger::Init(const char* logPath, int max_file_size)
{
	Property_[LOG_LEVEL_DEV] = LOG_STDERR | LOG_FILE;
	Property_[LOG_LEVEL_INF] = LOG_STDERR | LOG_FILE;
	Property_[LOG_LEVEL_WRN] = LOG_STDERR | LOG_FILE;
	Property_[LOG_LEVEL_ERR] = LOG_STDERR | LOG_FILE | LOG_SYSLOG;

	file_.logPath_ = logPath;
	file_.prefix_ = "NONAME";
	file_.filesize_ = 1024 * 1204 * 10;
	if(0 != max_file_size)
	{
		file_.filesize_ = max_file_size * 1024 * 1024;
	}
	if (file_.logPath_.length() > 0 && '/' == file_.logPath_[file_.logPath_.length() - 1])
	{
		file_.logPath_[file_.logPath_.length() - 1] = '\0';
	}

	if(false == boost::filesystem::exists(file_.logPath_))
	{
		boost::filesystem::path dir(file_.logPath_);
		if(false == boost::filesystem::create_directory(dir))
		{
			throw Exception(String("can't create directory for logging at \"", file_.logPath_, "\"").c_str());
		}
	}

	std::ifstream f("/proc/self/comm");
	getline(f, file_.prefix_);

	IsInit_ = true;
}

void Logger::SetLevelProperty(LOG_LEVEL_TYPE level, int flag)
{
	if(false == IsInit_)
	{
		throw Exception("set log level property exception, log is not initialized yet");
	}
	Property_[level] = flag;
}

}}
