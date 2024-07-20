module;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/locale.hpp>
#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#include <Winbase.h>
#endif
#include "../Library/Time/Time.h"
#include "../Library/Time/DateTime.h"

export module Gamnet.Log;

import Gamnet.String;
import Gamnet.Singleton;
import Gamnet.Exception;

import <fstream>;
import <string>;
import <limits>;
import <filesystem>;
import <cstdint>;
import <iostream>;
import <mutex>;

#undef max

namespace Gamnet::Log
{
	/// \brief log level
	export enum LOG_LEVEL_TYPE {
		LOG_LEVEL_MIN = 0,
		LOG_LEVEL_DEV, /**< debug log */
		LOG_LEVEL_INF, /**< information log */
		LOG_LEVEL_WRN, /**< warning log */
		LOG_LEVEL_ERR, /**< error log */
		LOG_LEVEL_MAX
	};

	export enum LOG_TYPE
	{
		LOG_STDERR = 0x00000001, /**< print to console */
		LOG_FILE = 0x00000010, /**< print to file */
		LOG_SYSLOG = 0x00000100	/**< print to sytem file */
	};

	class File
	{
	public:
		File();
		~File();

		std::ofstream& open(const Time::DateTime& now);

		std::ofstream ofstream_;
		std::string prefix_;
		std::string	filename_;
		std::string logPath_;
		Time::DateTime today_;
		size_t filesize_;
	};

	File::File() : filesize_(0)
	{
	}

	File::~File()
	{
		ofstream_.close();
	}
	std::ofstream& File::open(const Time::DateTime& now)
	{
		if (0 < today_.DateDiff(now))
		{
			ofstream_.close();
			today_ = now;

			boost::posix_time::ptime& ptime = today_;
			const std::string date = boost::gregorian::to_iso_extended_string(ptime.date());
			filename_ = logPath_ + "/" + prefix_ + "_" + date + ".txt";

			ofstream_.open(filename_.c_str(), std::fstream::out | std::fstream::app);
		}

		int filesize = 0;
		try
		{
			std::filesystem::path path = filename_.c_str();
			filesize = std::filesystem::file_size(path);
		}
		catch (std::filesystem::filesystem_error& /* e */)
		{
			filesize = std::numeric_limits<int>::max();
		}

		if (filesize_ < filesize)
		{
			ofstream_.close();
			today_ = now;

			boost::posix_time::ptime& ptime = today_;
			const std::string date = boost::gregorian::to_iso_extended_string(ptime.date()) + "_" + boost::posix_time::to_simple_string(ptime).substr(12, 8);
			filename_ = logPath_ + "/" + prefix_ + "_" + date + ".txt";

			ofstream_.open(filename_.c_str(), std::fstream::out | std::fstream::app);
		}

		return ofstream_;
	}

	export class Logger
	{
	public:
		Logger();
		~Logger() = default;

		/// \brief Initialize function for Logger lib
		/// \param logPath directory path log file will be create(recommend relative path)
		/// \return  return if true or false
		void Init(const std::string& log_path, const std::string& prefix, int max_file_size);
		void SetLevelProperty(LOG_LEVEL_TYPE level, int flag);
		template <class... Args>
		void Write(LOG_LEVEL_TYPE level, const Args&... args)
		{
			if (0 == (Property_[level] & (LOG_STDERR | LOG_FILE | LOG_SYSLOG)))
			{
				return;
			}

			if (false == IsInit_)
			{
				throw Gamnet::Exception(ErrorCode::NotInitializedError, "write log exception, log is not initialized yet");
			}

			Write(level, Format(args...));
		}
	private :
		void Write(LOG_LEVEL_TYPE level, const std::string& log);

	private:
		File file_;
		std::mutex lock_;

		int  Property_[LOG_LEVEL_MAX];
		bool IsInit_;
#ifdef _WIN32
		HANDLE console_;
		short dev_text_color;
		const short inf_text_color = 10;
		const short wrn_text_color = 14;
		const short err_text_color = 12;
#endif
	};

	export void Init(const std::string& log_dir = "log", const std::string& prefix = "log", size_t max_file_size = 5)
	{
		Singleton<Logger>::GetInstance().Init(log_dir, prefix, max_file_size);
	}

	export void ReadXml(const std::string& xml_path)
	{
		boost::property_tree::ptree ptree_;
		try {
			boost::property_tree::xml_parser::read_xml(xml_path, ptree_);
		}
		catch (const boost::property_tree::xml_parser_error& e)
		{
			std::cerr << "[Gamnet::Log] config file not found error(path:" << xml_path << ")" << std::endl;
			return;
		}

		try {
			std::string path = ptree_.get<std::string>("server.log.<xmlattr>.path");
			std::string prefix = ptree_.get<std::string>("server.log.<xmlattr>.prefix");
			int max_size = ptree_.get<int>("server.log.<xmlattr>.max_file_size");

			Init(path.c_str(), prefix.c_str(), max_size);
			auto log = ptree_.get_child("server.log");
			for (const auto& elmt : log)
			{
				LOG_LEVEL_TYPE level = LOG_LEVEL_TYPE::LOG_LEVEL_MIN;
				if ("dev" == elmt.first)
				{
					level = LOG_LEVEL_TYPE::LOG_LEVEL_DEV;
				}
				if ("inf" == elmt.first)
				{
					level = LOG_LEVEL_TYPE::LOG_LEVEL_INF;
				}
				if ("err" == elmt.first)
				{
					level = LOG_LEVEL_TYPE::LOG_LEVEL_ERR;
				}
				if ("wrn" == elmt.first)
				{
					level = LOG_LEVEL_TYPE::LOG_LEVEL_WRN;
				}
				if (LOG_LEVEL_TYPE::LOG_LEVEL_MIN == level)
				{
					continue;
				}

				int flag = 0;
				if ("yes" == elmt.second.get<std::string>("<xmlattr>.console"))
				{
					flag |= LOG_TYPE::LOG_STDERR;
				}
				if ("yes" == elmt.second.get<std::string>("<xmlattr>.file"))
				{
					flag |= LOG_TYPE::LOG_FILE;
				}
				if ("yes" == elmt.second.get<std::string>("<xmlattr>.sys"))
				{
					flag |= LOG_TYPE::LOG_SYSLOG;
				}

				Singleton<Logger>::GetInstance().SetLevelProperty(level, flag);
			}
		}
		catch (const boost::property_tree::ptree_bad_path& e)
		{
			throw Gamnet::Exception(ErrorCode::SystemInitializeError, e.what());
		}
	}

	export void SetLevelProperty(LOG_LEVEL_TYPE level, int flag)
	{
		Singleton<Logger>::GetInstance().SetLevelProperty(level, flag);
	}

	export template <class... Args>
	void Write(LOG_LEVEL_TYPE level, const Args&... args)
	{
		Singleton<Logger>::GetInstance().Write(level, args...);
	}

	Logger::Logger()
		: Property_{}
		, IsInit_(false)
#ifdef _WIN32
		, console_(INVALID_HANDLE_VALUE)
		, dev_text_color(0)
#endif
	{
	}

	void Logger::Init(const std::string& log_path, const std::string& prefix, int max_file_size)
	{
		Property_[LOG_LEVEL_DEV] = LOG_STDERR | LOG_FILE;
		Property_[LOG_LEVEL_INF] = LOG_STDERR | LOG_FILE;
		Property_[LOG_LEVEL_WRN] = LOG_STDERR | LOG_FILE;
		Property_[LOG_LEVEL_ERR] = LOG_STDERR | LOG_FILE | LOG_SYSLOG;

		file_.logPath_ = log_path;
		file_.prefix_ = prefix;
		file_.filesize_ = 1024 * 1204 * 10;
		if (0 != max_file_size)
		{
			file_.filesize_ = max_file_size * 1024 * 1024;
		}
		if (file_.logPath_.length() > 0 && '/' == file_.logPath_[file_.logPath_.length() - 1])
		{
			file_.logPath_[file_.logPath_.length() - 1] = '\0';
		}

		if (false == std::filesystem::exists(file_.logPath_))
		{
			std::filesystem::path dir(file_.logPath_);
			if (false == std::filesystem::create_directory(dir))
			{
				throw Gamnet::Exception(ErrorCode::CreateDirectoryFailError, "can't create directory for logging at \"", file_.logPath_, "\"");
			}
		}

#ifdef _WIN32
		console_ = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		dev_text_color = info.wAttributes;
#endif
		IsInit_ = true;
	}

	void Logger::SetLevelProperty(LOG_LEVEL_TYPE level, int flag)
	{
		if (false == IsInit_)
		{
			throw Gamnet::Exception(ErrorCode::NotInitializedError, "set log level property exception, log is not initialized yet");
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
			throw Gamnet::Exception(ErrorCode::NotInitializedError, "write log exception, log is not initialized yet");
		}

		Time::DateTime now(Time::Local::Now());
		const std::string str = "[" + now.ToString() + "] " + log;

		std::lock_guard<std::mutex> lo(lock_);
		if (Property_[level] & LOG_STDERR)
		{
			switch (level)
			{
			case LOG_LEVEL_ERR:
#ifdef _WIN32
				SetConsoleTextAttribute(console_, err_text_color);
#endif
				std::cerr << str << std::endl;
				break;
			case LOG_LEVEL_WRN:
#ifdef _WIN32
				SetConsoleTextAttribute(console_, wrn_text_color);
#endif
				std::cout << str << std::endl;
				break;
			case LOG_LEVEL_INF:
#ifdef _WIN32
				SetConsoleTextAttribute(console_, inf_text_color);
#endif
				std::cout << str << std::endl;
				break;
			case LOG_LEVEL_DEV:
#ifdef _WIN32
				SetConsoleTextAttribute(console_, dev_text_color);
#endif
				std::cout << str << std::endl;
				break;
			}
		}

		if (Property_[level] & LOG_FILE)
		{
			std::ofstream& ofstream_ = file_.open(now);
			ofstream_ << time << " " << log.c_str() << std::endl;
		}
	}
}