#ifndef GAMNET_LOG_LOGGER_H_
#define GAMNET_LOG_LOGGER_H_

#include <memory>
#include <sstream>
#include <iostream>
#include <mutex>
#include "../Library/String.h"
#include "../Library/Exception.h"
#include "File.h"

namespace Gamnet { namespace Log {

class Logger {
public :
	/// \brief log level
	enum LOG_LEVEL_TYPE {
		LOG_LEVEL_MIN = 0,
		LOG_LEVEL_DEV, /**< debug log */
		LOG_LEVEL_INF, /**< information log */
		LOG_LEVEL_WRN, /**< warning log */
		LOG_LEVEL_ERR, /**< error log */
		LOG_LEVEL_MAX
	};

	enum LOG_TYPE
	{
		LOG_STDERR = 	0x00000001, /**< print to console */
		LOG_FILE   =	0x00000010, /**< print to file */
		LOG_SYSLOG = 	0x00000100	/**< print to sytem file */
	};
private :
	int  Property_[LOG_LEVEL_MAX];
	bool IsInit_;
	bool timeRecord_;
	File file_;
	std::mutex mutex_;
#ifdef _WIN32
	HANDLE console_;
	short dev_text_color;
	const short inf_text_color = 10;
	const short wrn_text_color = 14;
	const short err_text_color = 12;
#endif
public :
	Logger() : IsInit_(false), timeRecord_(true){};
	virtual ~Logger(){};

	static Logger& GetInstance()
	{
		static Logger self;
		return self;
	}

	void SetTimeRecord(bool record) { timeRecord_ = record; }

	template <typename... Args>
	void Write(LOG_LEVEL_TYPE level, const Args&... args)
	{
		if(0 == (Property_[level]&(LOG_STDERR | LOG_FILE | LOG_SYSLOG)))
		{
			return;
		}

		if(false == IsInit_)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NotInitializedError, "write log exception, log is not initialized yet");
		}

		Write(level, Format(args...));
	}

	/// \brief Initialize function for Logger lib
	/// \param logPath directory path log file will be create(recommend relative path)
	/// \return  return if true or false
	void Init(const char* logPath, const char* prefix, int max_file_size);
	void SetLevelProperty(LOG_LEVEL_TYPE level, int flag);
	void Write(LOG_LEVEL_TYPE level, const std::string& log);
};

}}

/**
 * @name macro for Logging
 * @brief ...
 * @code
 * 	#include "../../Logger/Static.h"
 * 	#include <unistd.h>
 *
 * 	int main()
 * 	{
 * 		if(false == Logger::Init())
 * 		{
 * 			return 0;
 * 	    }
 *
 * 	    Logger::SetLevelProperty(Logger::LOG_LEVEL_DEV, Logger::LOG_STDERR | Logger::LOG_FILE);
 * 	    Logger::SetMaxFileSize(10); //MB
 *
 * 	    int some_integer = 12345;
 * 	    char* some_char_ptr = "67890";
 * 	    std::string some_string = "Hello!";
 *
 *		LOG(DEV, "This is \"DEV\" log message(print_int:", some_integer, ", char*_print:", some_char_ptr, ", std::string_print:", some_string, ")");
 *		LOG(INF, "This is \"INF\" log message(print_int:", some_integer, ", char*_print:", some_char_ptr, ", std::string_print:", some_string, ")");
 *		LOG(ERR, "This is \"ERR\" log message(print_int:", some_integer, ", char*_print:", some_char_ptr, ", std::string_print:", some_string, ")");
 *
 *		sleep(1);
 *		return 0;
 *	}
 * @endcode
 */

#endif /* LOGGER_LOG_H_ */
