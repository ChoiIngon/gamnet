#ifndef _GAMNET_LIB_META_DATA_H_
#define _GAMNET_LIB_META_DATA_H_

#include <map>
#include <string>
#include <functional>
#include <memory>
#include <fstream>
#include <list>
#include "Time/DateTime.h"
#include "Json/json.h"
#include "Exception.h"

namespace Gamnet {

class MetaData
{
	typedef void(MetaData::*custom_bind)(const std::string&);
	std::map<std::string, std::function<void(const std::string&)>>	bind_functions;
protected:
	void Bind(const std::string& name, bool& member);
	void Bind(const std::string& name, int16_t& member);
	void Bind(const std::string& name, uint16_t& member);
	void Bind(const std::string& name, int32_t& member);
	void Bind(const std::string& name, uint32_t& member);
	void Bind(const std::string& name, int64_t& member);
	void Bind(const std::string& name, uint64_t& member);
	void Bind(const std::string& name, float& member);
	void Bind(const std::string& name, double& member);
	void Bind(const std::string& name, std::string& member);
	void Bind(const std::string& name, Time::DateTime& member);
	template <class T>
	void Bind(const std::string& name, std::vector<T>& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member.push_back(boost::lexical_cast<T>(value));
		}));
	}
	template <class F>
	void CustomBind(const std::string& name, F f)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), f));
	}
public:
	void Init(const Json::Value& row);
	virtual bool OnLoad();
};

template <class T>
class MetaReader
{
	std::list<std::shared_ptr<T>> meta_datas;

public :
	const std::list<std::shared_ptr<T>>& Read(const std::string& filePath)
	{
		meta_datas.clear();
		std::ifstream file(filePath);
		if(true == file.fail())
		{
			throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, "[Gamnet::MetaData] can not find meta file(path:", filePath, ")");
		}

		std::string	line;
		std::vector<std::string>	columns;

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
			size_t pos = 0;
			pos = line.find_last_of('\n');
			if (std::string::npos != pos)
			{
				line = line.substr(0, pos);
			}

			pos = line.find_last_of('\r');
			if(std::string::npos != pos)
			{
				line = line.substr(0, pos);
			}
			
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

			std::shared_ptr<T> meta = std::make_shared<T>();
			try {
				meta->Init(row);
			}
			catch (const Exception& e)
			{
				throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, "file:", filePath, ", column:", e.what());
			}
			if(false == meta->OnLoad())
			{
				Json::FastWriter writer;
				throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, writer.write(row));
			}
			meta_datas.push_back(meta);
		}
		// This checks for a trailing comma with no data after it.
		
		return meta_datas;
	}
	const std::list<std::shared_ptr<T>>& MetaDatas() const
	{
		return meta_datas;
	}
};

#define GAMNET_META_MEMBER(member) \
	Bind(#member, member)

#define GAMNET_META_CUSTOM(member, func_ptr) \
	CustomBind(#member, std::bind(&func_ptr, this, std::ref(member), std::placeholders::_1))
}
#endif
