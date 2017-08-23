#ifndef GAMNET_LOG_LOG_H_
#define GAMNET_LOG_LOG_H_

#include "Logger.h"

namespace Gamnet { namespace Log {
	/// \brief Initialize function for Logger lib
	/// \param log_dir the directory that log file will be created(relative directory path would be recommened)
	void Init(const char* log_dir = "log", const char* prefix = "log", int max_file_size = 5);
	void ReadXml(const char* xml_path);
	/// \brief 로그 레벨 별 출력 여부와 out direction 설정 함수
	/// \param level property 셋팅 할 로그 레벨
	/// \param flag stdout, file, syslog 등 로그 출력 방향. 0일 경우 해당 레벨은 로깅하지 않음
	void SetLevelProperty(Logger::LOG_LEVEL_TYPE level, int flag);
	template <typename... Args>
	void Write(Logger::LOG_LEVEL_TYPE level, const Args&... args)
	{
		Logger::GetInstance().Write(level, args...);
	}
}}

#ifdef _WIN32
#define GAMNET_LOG Gamnet::Log::Write
#define GAMNET_DEV Gamnet::Log::Logger::LOG_LEVEL_DEV, "DEV [", __FILE__, ":", __FUNCTION__, "@" , __LINE__, "] "
#define GAMNET_INF Gamnet::Log::Logger::LOG_LEVEL_INF, "INF "
#define GAMNET_WRN Gamnet::Log::Logger::LOG_LEVEL_WRN, "WRN [", __FILE__, ":", __FUNCTION__, "@" , __LINE__, "] "
#define GAMNET_ERR Gamnet::Log::Logger::LOG_LEVEL_ERR, "ERR [", __FILE__, ":", __FUNCTION__, "@" , __LINE__, "] "
#else
#define GAMNET_LOG Gamnet::Log::Write
#define GAMNET_DEV Gamnet::Log::Logger::LOG_LEVEL_DEV, "DEV [", __FILE__, ":", __func__, "@" , __LINE__, "] "
#define GAMNET_INF Gamnet::Log::Logger::LOG_LEVEL_INF, "INF "
#define GAMNET_WRN Gamnet::Log::Logger::LOG_LEVEL_WRN, "WRN [", __FILE__, ":", __func__, "@" , __LINE__, "] "
#define GAMNET_ERR Gamnet::Log::Logger::LOG_LEVEL_ERR, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] "
#endif
#define LOG GAMNET_LOG
#define DEV GAMNET_DEV
#define INF GAMNET_INF
#define WRN GAMNET_WRN
#define ERR GAMNET_ERR
#endif /* LOG_H_ */
