#include "MetaData.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>  
#include <idl/MessageCommon.h>
#include <Gamnet/Library/Exception.h>

void MetaData::Bind(const std::string& name, bool& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, int16_t& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, uint16_t& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, int32_t& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, uint32_t& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, int64_t& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, uint64_t& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, float& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, double& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, std::string& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, Gamnet::Time::DateTime& member)
{
	bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Init(const Json::Value& row)
{
	for(auto& cell : row["cells"])
	{
		const Json::Value header = cell["header"];
		const std::string& key = header["name"].asString();
		const std::string& value = cell["value"].asString();
		if("" == value)
		{
			continue;
		}
		if(bind_functions.end() == bind_functions.find(key))
		{
			continue;
		}
		try {
			bind_functions[key](cell);
		}
		catch(const boost::bad_lexical_cast& /*e*/)
		{
			throw Gamnet::Exception((int)Message::ErrorCode::UndefineError, "[MetaData] meta data load fail(file:", row["file"].asString(), ", name:", key, ", row_num:", row["row_num"].asInt(), ", reason:bed lexical cast)");
		}
		catch(const Gamnet::Exception& e)
		{
			throw Gamnet::Exception((int)Message::ErrorCode::UndefineError, "[MetaData] meta data load fail(file:", row["file"].asString(), ", name:", key, ", row_num:", row["row_num"].asInt(), ", reason:", e.what(), ")");
		}
	}
	//bind_functions.clear();
}

void MetaData::OnLoad() 
{
}

void MetaData::Allocation(bool& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	if ("false" == boost::algorithm::to_lower_copy(value))
	{
		member = false;
		return;
	}
	else if ("true" == boost::algorithm::to_lower_copy(value))
	{
		member = true;
		return;
	}
	member = boost::lexical_cast<bool>(value);
}

void MetaData::Allocation(int16_t& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = boost::lexical_cast<int16_t>(value);
}

void MetaData::Allocation(uint16_t& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = boost::lexical_cast<uint16_t>(value);
}

void MetaData::Allocation(int32_t& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = boost::lexical_cast<int32_t>(value);
}

void MetaData::Allocation(uint32_t& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = boost::lexical_cast<uint32_t>(value);
}

void MetaData::Allocation(int64_t& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = boost::lexical_cast<int64_t>(value);
}

void MetaData::Allocation(uint64_t& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = boost::lexical_cast<uint64_t>(value);
}

void MetaData::Allocation(float& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = boost::lexical_cast<float>(value);
}

void MetaData::Allocation(double& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = boost::lexical_cast<double>(value);
}

void MetaData::Allocation(std::string& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = value;
}

void MetaData::Allocation(Gamnet::Time::DateTime& member, const Json::Value& cell)
{
	const std::string& value = cell["value"].asString();
	member = value;
}

void MetaData::Allocation(MetaData& member, const Json::Value& cell)
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
