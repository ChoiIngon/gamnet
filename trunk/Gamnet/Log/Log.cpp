/*
 * Log.cpp
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#include "Logger.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Log {

void Init(const char* log_dir, int max_file_size)
{
	Singleton<Logger>().Init(log_dir, max_file_size);
}
void SetLevelProperty(Logger::LOG_LEVEL_TYPE level, int flag)
{
	Singleton<Logger>().SetLevelProperty(level, flag);
}

}}


