#ifndef _META_DATA_H_
#define _META_DATA_H_

#include <functional>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <Gamnet/Library/Time/DateTime.h>
#include <Gamnet/Library/Json/json.h>
#include <Gamnet/Library/Exception.h>
#include "CSVReader.h"

class MetaData
{
public:
	void Init(const Json::Value& row);
	virtual void OnLoad();

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
	void Bind(const std::string& name, Gamnet::Time::DateTime& member);
	template <class T>
	void Bind(const std::string& name, T& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& value) {
			this->Allocation(member, value);
		}));
	}
	template <class T>
	void Bind(const std::string& name, std::vector<T>& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& value) {
			if ("" == value["value"].asString())
			{
				return;
			}
			int index = value["header"]["index"].asInt();
			if((int)member.size() <= index)
			{
				member.resize(index + 1);
			}
			T& elmt = member[index];
			Allocation(elmt, value);
		}));
	}
	/*
		F = void(T& member, const std::string& value)
	*/
	template <class F>
	void CustomBind(const std::string& name, F f)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [f](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			f(value);
		}));
	}
private :
	void Allocation(bool& member, const Json::Value& cell);
	void Allocation(int16_t& member, const Json::Value& cell);
	void Allocation(uint16_t& member, const Json::Value& cell);
	void Allocation(int32_t& member, const Json::Value& cell);
	void Allocation(uint32_t& member, const Json::Value& cell);
	void Allocation(int64_t& member, const Json::Value& cell);
	void Allocation(uint64_t& member, const Json::Value& cell);
	void Allocation(float& member, const Json::Value& cell);
	void Allocation(double& member, const Json::Value& cell);
	void Allocation(std::string& member, const Json::Value& cell);
	void Allocation(Gamnet::Time::DateTime& member, const Json::Value& cell);
	template <class T>
	void Allocation(T& member, const Json::Value& cell)
	{
		Json::Value row;
		row["file"] = cell["file"];
		row["row_num"] = cell["row_num"];

		Json::Value column;
		column["header"] = cell["header"]["children"];
		column["value"] = cell["value"];
		row["cells"].append(column);

		member.Init(row);
	}
	template <class T>
	void Allocation(std::shared_ptr<T>& member, const Json::Value& cell)
	{
		Json::Value row;
		row["file"] = cell["file"];
		row["row_num"] = cell["row_num"];

		Json::Value column;
		column["header"] = cell["header"]["children"];
		column["value"] = cell["value"];
		row["cells"].append(column);

		if(nullptr == member)
		{
			member = std::make_shared<T>();
		}
		member->Init(row);
	}
private:
	std::map<std::string, std::function<void(const Json::Value&)>>	bind_functions;
};

template <class T>
class MetaReader
{
public :
	typedef std::vector<std::shared_ptr<T>> MetaDatas;
	const MetaDatas& Read(const std::string& filePath)
	{
		CSVReader csv;
		csv.ReadFile(filePath);
		meta_datas.clear();

		Json::Value headers;
		// read column
		for(const std::string& cell : csv.GetColumnNames())
		{
			headers.append(ReadColumnName(cell));
		}
		
		// read data rows
		int rowNum = 1;
		for(auto itr : csv)
		{
			Json::Value row;
			row["file"] = filePath;
			row["row_num"] = rowNum;

			bool emptyRow = true;
			for(int i=0; i< csv.GetColumnNames().size(); i++)
			{
				if ("" != itr.GetValue(i))
				{
					emptyRow = false;
				}

				Json::Value column;
				column["header"] = headers[i];
				column["value"] = itr.GetValue(i);
				row["cells"].append(column);
			}

			if(false == emptyRow)
			{
				std::shared_ptr<T> meta = std::make_shared<T>();
				meta->Init(row);
				meta->OnLoad();
				meta_datas.push_back(meta);
			}
			rowNum++;
		}
		return meta_datas;
	}
	const MetaDatas& GetAllMetaData() const
	{
		return meta_datas;
	}

	Json::Value ReadColumnName(const std::string& cell)
	{
		Json::Value root;
		const std::string cellValue = boost::algorithm::to_lower_copy(cell);
		std::string column = cellValue; 
		std::size_t dotPos = cellValue.find('.');
		if (std::string::npos != dotPos) // cellValue has '.'. it means this column is hierarchy
		{
			column = cellValue.substr(0, dotPos);
		}

		std::size_t braceStartPos = column.find('[');
		std::size_t braceEndPos = column.find(']');
		if (std::string::npos != braceStartPos && std::string::npos != braceEndPos) // column has '[' and  ']'. it means column is array
		{
			root["index"] = boost::lexical_cast<int>(column.substr(braceStartPos + 1, braceEndPos - braceStartPos - 1));
		}

		std::size_t columnEndPos = std::min(dotPos, braceStartPos);
		root["name"] = column.substr(0, columnEndPos);
		if (std::string::npos != dotPos)
		{
			root["children"] = ReadColumnName(cellValue.substr(dotPos + 1));
		}
		return root;
	}

	void ReadColumnType(Json::Value& header, const std::string& cell)
	{
		if(true == header["children"].isNull())
		{
			header["type"] = cell;
			return;
		}
		ReadColumnType(header["children"], cell);
	}
private :
	MetaDatas meta_datas;
};

#define META_MEMBER(member) \
	Bind(#member, member)

#define META_CUSTOM(member, func_ptr) \
	CustomBind(#member, std::bind(&func_ptr, this, std::ref(member), std::placeholders::_1))

#endif
