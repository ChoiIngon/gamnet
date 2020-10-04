#include "MetaData.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>  
#include "Exception.h"

namespace Gamnet 
{
	void MetaData::Bind(const std::string& name, bool& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if("" == value)
			{
				return;
			}
			if("false" == boost::algorithm::to_lower_copy(value))
			{
				member = false;
				return;
			}
			else if("true" == boost::algorithm::to_lower_copy(value))
			{ 
				member = true;
				return;
			}
			member = boost::lexical_cast<bool>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, int16_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			member = boost::lexical_cast<int16_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, uint16_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			member = boost::lexical_cast<uint16_t>(value);
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
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			member = boost::lexical_cast<uint32_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, int64_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			member = boost::lexical_cast<int64_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, uint64_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			member = boost::lexical_cast<uint64_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, float& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			member = boost::lexical_cast<float>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, double& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			member = boost::lexical_cast<double>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, std::string& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const Json::Value& cell) {
			this->Allocation(member, cell);
		}));
	}

	void MetaData::Bind(const std::string& name, Time::DateTime& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const Json::Value& cell) {
			const std::string& value = cell["value"].asString();
			if ("" == value)
			{
				return;
			}
			member = value;
		}));
	}

	void MetaData::Init(const Json::Value& row)
	{
		for(auto& cell : row["cells"])
		{
			const Json::Value header = cell["header"];
			const std::string& key = header["name"].asString();
			//const std::string& value = cell["value"].asString();
			if(bind_functions.end() == bind_functions.find(key))
			{
				continue;
			}
			try {
				bind_functions[key](cell);
			}
			catch(const boost::bad_lexical_cast& e)
			{
				throw Exception(ErrorCode::SystemInitializeError, "[Gamnet::MetaData] meta data load fail(file:", row["file"].asString(), ", name:", key, ", row_num:", row["row_num"].asInt(), ", reason:bed lexical cast)");
			}
			catch(const Exception& e)
			{
				throw Exception(ErrorCode::SystemInitializeError, "[Gamnet::MetaData] meta data load fail(file:", row["file"].asString(), ", name:", key, ", row_num:", row["row_num"].asInt(), ", reason:", e.what(), ")");
			}
		}
		//bind_functions.clear();
	}

	bool MetaData::OnLoad() 
	{
		return true;
	}

	void MetaData::Allocation(int32_t& member, const Json::Value& cell)
	{
		const std::string& value = cell["value"].asString();
		if ("" == value)
		{
			return;
		}
		member = boost::lexical_cast<int32_t>(value);
	}
	void MetaData::Allocation(std::string& member, const Json::Value& cell)
	{
		const std::string& value = cell["value"].asString();
		member = value;
	}
}
