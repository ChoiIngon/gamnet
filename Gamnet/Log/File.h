/*
 * Log.h
 *
 *  Created on: 2012. 8. 17.
 *      Author: jjaehuny
 */

#ifndef GAMNET_LOG_FILE_H
#define GAMNET_LOG_FILE_H

#include <fstream>
#include <string>

namespace Gamnet { namespace Log {
struct File
{
	File();
	~File();
	std::ofstream& open(const tm& now);

	std::ofstream ofstream_;
	std::string prefix_;
	std::string	filename_;
	std::string logPath_;
	tm	today_;
	int filesize_;
};

}} /* Logger */

#endif /* LOGGER_NLOG_FILE_H */
