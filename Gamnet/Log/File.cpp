#include "File.h"
#include <boost/filesystem.hpp>
#include <limits>

namespace Gamnet { namespace Log {

File::File() : filesize_(0)
{
	::memset(&today_, 0, sizeof(tm));
}

File::~File()
{
	ofstream_.close();
}

std::ofstream& File::open(const tm& now)
{
	if(today_.tm_year != now.tm_year || today_.tm_yday != now.tm_yday)
	{
		ofstream_.close();
		char datebuf[10] = {0};
#ifdef _WIN32
		_snprintf_s(datebuf, 10, "%04d%02d%02d", now.tm_year+1900, now.tm_mon+1, now.tm_mday);
#else
		snprintf(datebuf, 10, "%04d%02d%02d", now.tm_year+1900, now.tm_mon+1, now.tm_mday);
#endif
		today_ = now;
		filename_ = logPath_ + "/" + prefix_ + "_" + std::string(datebuf) + ".txt";
		ofstream_.open(filename_.c_str(), std::fstream::out | std::fstream::app);
	}

	int filesize = 0;
	try
	{
		filesize = boost::filesystem::file_size(filename_.c_str());
	}
	catch(boost::filesystem::filesystem_error& /* e */)
	{
		filesize = std::numeric_limits<int>::max();
	}
	if(filesize_ < filesize)
	{
		ofstream_.close();
		char datebuf[20] = {0};
#ifdef _WIN32
		_snprintf_s(datebuf, 20, "%04d%02d%02d_%02d%02d%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
#else
		snprintf(datebuf, 20, "%04d%02d%02d_%02d%02d%02d", now.tm_year+1900, now.tm_mon+1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
#endif
		today_ = now;
		filename_ = logPath_ + "/" + prefix_ + "_" + std::string(datebuf) + ".txt";
		ofstream_.open(filename_.c_str(), std::fstream::out | std::fstream::app);
	}

	return ofstream_;
}

}} /* Logger */
