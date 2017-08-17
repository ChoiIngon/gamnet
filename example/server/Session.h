/*
 * Session.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef SESSION_H_
#define SESSION_H_

#include "../../Gamnet/Gamnet.h"
#include "message.h"

class Session : public Gamnet::Network::Session {
public:
	Session();
	virtual ~Session();
	virtual void OnAccept();
	virtual void OnClose(int reason);

	std::shared_ptr<UserData> user_data;
};

class TestSession : public Gamnet::Test::Session {
public :
	UserData user_data;
};

#ifdef _WIN32
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

#ifdef _WIN32
#define ERROR(error) error, "ERR [", __FILE__, ":", __FUNCTION__, "@" , __LINE__, "] ", #error, " "

#else
#define ERROR(error) error, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] ", #error, " "
#endif

#endif /* SESSION_H_ */
