#ifndef _GAMNET_LIB_META_DATA_H_
#define _GAMNET_LIB_META_DATA_H_

#include <map>
#include <string>
#include <functional>
#include <memory>
#include <list>
#include "Time/DateTime.h"
#include "Json/json.h"

namespace Gamnet {

class MetaData
{
	std::map<std::string, std::function<void(const std::string&)>>	bind_functions;
protected:
	bool			Bind(const std::string& name, bool& member);
	int16_t			Bind(const std::string& name, int16_t& member);
	uint16_t		Bind(const std::string& name, uint16_t& member);
	int32_t			Bind(const std::string& name, int32_t& member);
	uint32_t		Bind(const std::string& name, uint32_t& member);
	int64_t			Bind(const std::string& name, int64_t& member);
	uint64_t		Bind(const std::string& name, uint64_t& member);
	float			Bind(const std::string& name, float& member);
	double			Bind(const std::string& name, double& member);
	std::string		Bind(const std::string& name, std::string& member);
	Time::DateTime	Bind(const std::string& name, Time::DateTime& member);
	template <class T>
	std::vector<T>	Bind(const std::string& name, std::vector<T>& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member.push_back(boost::lexical_cast<T>(value));
		}));
		return std::vector<T>();
	}
public:
	void Init(const Json::Value& row);
};

template <class T>
class MetaDataReader
{
	std::list<std::shared_ptr<T>> meta_datas;

public :
	const std::list<std::shared_ptr<T>> Read(const std::string& filePath)
	{
		std::ifstream file(filePath);

		std::string	line;
		std::vector<std::string>	columns;

		std::list<std::shared_ptr<T>> result;
		// read column
		{
			std::getline(file, line);
			std::stringstream lineStream(line);
			std::string cell;
			
			while (std::getline(lineStream, cell, ','))
			{
				boost::algorithm::to_lower(cell);
				std::size_t pos = cell.find('[');
				cell = cell.substr(0, pos);
				columns.push_back(cell);
			}
		}

		// skip data type
		std::getline(file, line);

		// read data rows
		while(std::getline(file, line))
		{
			std::stringstream lineStream(line);
			std::string cell;
		
			Json::Value row;
			int index = 0;
			while (std::getline(lineStream, cell, ','))
			{
				Json::Value value;
				value[columns[index++]] = cell;
				row.append(value);
			}

			if (!lineStream && cell.empty())
			{
				continue;
			}

			Json::FastWriter writer;
			std::cout << writer.write(row) << std::endl;

			std::shared_ptr<T> meta = std::make_shared<T>();
			meta->Init(row);
			result.push_back(meta);
		}
		// This checks for a trailing comma with no data after it.
		
		return result;
	}
};

#define GAMNET_INIT_MEMBER(member) \
	member(Bind(#member, member))

}
#endif
