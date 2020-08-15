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
public :
	typedef std::vector<std::shared_ptr<T>> MetaDatas;
	const MetaDatas& Read(const std::string& filePath)
	{
		meta_datas.clear();
		std::ifstream file(filePath);
		if(true == file.fail())
		{
			throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, "[Gamnet::MetaData] can not find meta file(path:", filePath, ")");
		}

		std::string	line;
		std::string cell;
		std::vector<std::string>	columnNames;

		// read column
		{
			std::getline(file, line);
			std::stringstream lineStream(line);
			
			while (std::getline(lineStream, cell, ','))
			{
				boost::algorithm::to_lower(cell);
				std::size_t pos = cell.find('[');
				cell = cell.substr(0, pos);
				columnNames.push_back(cell);
			}
		}

		// skip data type
		{
			std::getline(file, line);
		}

		// read data rows
		{
			int rowNum = 1;
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
		
				Json::Value row;
				row["file"] = filePath;
				row["row_num"] = rowNum;
				int index = 0;
				while (std::getline(lineStream, cell, ','))
				{
					Json::Value column;
					column["key"] = columnNames[index++];
					column["value"] = cell;
					row["cells"].append(column);
				}

				std::shared_ptr<T> meta = std::make_shared<T>();
				meta->Init(row);
				if(false == meta->OnLoad())
				{
					throw Exception(ErrorCode::SystemInitializeError, "[Gamnet::MetaData] OnLoad fail(file:", filePath, ", row_num:", rowNum, ")");
				}
				meta_datas.push_back(meta);
				rowNum++;
			}
		}
		
		return meta_datas;
	}
	const MetaDatas& GetAllMetaData() const
	{
		return meta_datas;
	}
private :
	MetaDatas meta_datas;
};

#define GAMNET_META_MEMBER(member) \
	Bind(#member, member)

#define GAMNET_META_CUSTOM(member, func_ptr) \
	CustomBind(#member, std::bind(&func_ptr, this, std::ref(member), std::placeholders::_1))
}
#endif
