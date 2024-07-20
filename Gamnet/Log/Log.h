#ifndef GAMNET_LOG_LOG_H_
#define GAMNET_LOG_LOG_H_

import Gamnet.Log;

#define __FILENAME__ (std::strrchr(__FILE__,'\\')+1)
#define GAMNET_LOG Gamnet::Log::Write
#define GAMNET_DEV Gamnet::Log::LOG_LEVEL_TYPE::LOG_LEVEL_DEV, "DEV [", __FILENAME__, ":", __func__, "@" , __LINE__, "] "
#define GAMNET_INF Gamnet::Log::LOG_LEVEL_TYPE::LOG_LEVEL_INF, "INF "
#define GAMNET_WRN Gamnet::Log::LOG_LEVEL_TYPE::LOG_LEVEL_WRN, "WRN "
#define GAMNET_ERR Gamnet::Log::LOG_LEVEL_TYPE::LOG_LEVEL_ERR, "ERR [", __FILENAME__, ":", __func__, "@" , __LINE__, "] "

#define LOG GAMNET_LOG
#define DEV GAMNET_DEV
#define INF GAMNET_INF
#define WRN GAMNET_WRN
#define ERR GAMNET_ERR

#endif /* LOG_H_ */
