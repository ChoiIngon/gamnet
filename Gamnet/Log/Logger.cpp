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

void Logger::Init(const char* logPath, const char* prefix, int max_file_size)
{
	Property_[LOG_LEVEL_DEV] = LOG_STDERR | LOG_FILE;
	Property_[LOG_LEVEL_INF] = LOG_STDERR | LOG_FILE;
	Property_[LOG_LEVEL_WRN] = LOG_STDERR | LOG_FILE;
	Property_[LOG_LEVEL_ERR] = LOG_STDERR | LOG_FILE | LOG_SYSLOG;

	file_.logPath_ = logPath;
	file_.prefix_ = prefix;
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
			throw GAMNET_EXCEPTION(ErrorCode::CreateDirectoryFailError, "can't create directory for logging at \"", file_.logPath_, "\"");
		}
	}

	IsInit_ = true;
}

void Logger::SetLevelProperty(LOG_LEVEL_TYPE level, int flag)
{
	if(false == IsInit_)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NotInitializedError, "set log level property exception, log is not initialized yet");
	}
	Property_[level] = flag;
}

void Logger::Write(LOG_LEVEL_TYPE level, const std::string& log)
{
	if (0 == (Property_[level] & (LOG_STDERR | LOG_FILE | LOG_SYSLOG)))
	{
		return;
	}

	if (false == IsInit_)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NotInitializedError, "write log exception, log is not initialized yet");
	}

	time_t logtime_;
	struct tm when;
	char timebuf[22] = { 0 };
	time(&logtime_);
#ifdef _WIN32 // build Static multithreaded library "libcmt"
	localtime_s(&when, &logtime_);
	_snprintf_s(timebuf, 22, 21, "[%04d-%02d-%02d %02d:%02d:%02d]", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#else
	localtime_r(&logtime_, &when);
	snprintf(timebuf, 22, "[%04d-%02d-%02d %02d:%02d:%02d]", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#endif

	std::lock_guard<std::mutex> lo(mutex_);
	if (Property_[level] & LOG_STDERR)
	{
		if (level == LOG_LEVEL_ERR)
		{
			std::cerr << timebuf << " " << log.c_str() << std::endl;
		}
		else
		{
			std::cout << timebuf << " " << log.c_str() << std::endl;
		}
	}
	if (Property_[level] & LOG_FILE)
	{
		std::ofstream& ofstream_ = file_.open(when);
		if (true == timeRecord_)
		{
			ofstream_ << timebuf << " " << log.c_str() << std::endl;
		}
		else
		{
			ofstream_ << log.c_str() << std::endl;
		}
	}
}

}}
