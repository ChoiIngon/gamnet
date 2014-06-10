#ifndef GAMNET_LOG_LOGGER_H_
#define GAMNET_LOG_LOGGER_H_

#include <memory>
#include <sstream>
#include <iostream>
#include <mutex>
#include "../Library/Singleton.h"
#include "../Library/Exception.h"
#include "File.h"

namespace Gamnet { namespace Log {

class Logger {
public :
	/// \brief 로그 레벨
	enum LOG_LEVEL_TYPE {
		LOG_LEVEL_MIN = 0,
		LOG_LEVEL_DEV, /**< 디버깅 로그 */
		LOG_LEVEL_INF, /**< 일반 로그 */
		LOG_LEVEL_WRN,
		LOG_LEVEL_ERR, /**<  에러 로그 */
		LOG_LEVEL_MAX
	};

	enum LOG_TYPE
	{
		LOG_STDERR = 	0x00000001, /**< 표준 출력 */
		LOG_FILE   =	0x00000010, /**< 파일 출력 */
		LOG_SYSLOG = 	0x00000100 /**< syslog 출력 */
	};
private :
	int  Property_[LOG_LEVEL_MAX];
	bool IsInit_;
	File file_;
	std::mutex mutex_;
public :
	Logger() : IsInit_(false){};
	virtual ~Logger(){};

	template <typename... Args>
	void Write(LOG_LEVEL_TYPE level, const Args&... args)
	{
		if(0 == (Property_[level]&(LOG_STDERR | LOG_FILE | LOG_SYSLOG)))
		{
			return;
		}
		String s(args...);
		if(false == IsInit_)
		{
			throw Exception("write log exception, log is not initialized yet");
		}

		time_t logtime_;
		struct tm when;
		time(&logtime_);
		localtime_r( &logtime_, &when );

		char timebuf[22] = {0};
		snprintf (timebuf, 22, "[%04d-%02d-%02d %02d:%02d:%02d]", when.tm_year+1900, when.tm_mon+1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);

		std::lock_guard<std::mutex> lo(mutex_);
		if(Property_[level]&LOG_STDERR)
		{
			if(level == LOG_LEVEL_ERR )
			{
				std::cerr << timebuf << " " << s.c_str() << std::endl;
			}
			else
			{
				std::cout << timebuf << " " <<  s.c_str() << std::endl;
			}
		}
		if(Property_[level]&LOG_FILE)
		{
			std::ofstream& ofstream_ = Singleton<File>().open(when);
			ofstream_ << timebuf << " " << s.c_str() << std::endl;
		}
	}

	/// \brief Initialize function for Logger lib
	/// \param logPath 로그 파일이 생성될 디렉토리(상대 경로 사용할 것을 권고)
	/// \return  성공시 true 리턴, 실패 false. 세부 에러 내용은 errno 체크
	void Init(const char* logPath = "log", int max_file_size = 0);
	/// \brief 로그 레벨 별 출력 여부와 out direction 설정 함수
	/// \param level property 셋팅 할 로그 레벨
	/// \param flag stdout, file, syslog 등 로그 출력 방향. 0일 경우 해당 레벨은 로깅하지 않음
	void SetLevelProperty(LOG_LEVEL_TYPE level, int flag);
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
