#include "MetaData.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>  
#include "Exception.h"

namespace Gamnet 
{
	void MetaData::Bind(const std::string& name, bool& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
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
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = boost::lexical_cast<int16_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, uint16_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = boost::lexical_cast<uint16_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, int32_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = boost::lexical_cast<int32_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, uint32_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = boost::lexical_cast<uint32_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, int64_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = boost::lexical_cast<int64_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, uint64_t& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = boost::lexical_cast<uint64_t>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, float& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = boost::lexical_cast<float>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, double& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = boost::lexical_cast<double>(value);
		}));
	}

	void MetaData::Bind(const std::string& name, std::string& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = value;
		}));
	}

	void MetaData::Bind(const std::string& name, Time::DateTime& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [&member](const std::string& value) {
			member = value;
		}));
	}

	void MetaData::Init(const Json::Value& row)
	{
		for(auto& cell : row)
		{
			Json::Value::Members members = cell.getMemberNames();
			for(auto& name : members)
			{
				if(bind_functions.end() == bind_functions.find(name))
				{
					throw Exception(ErrorCode::SystemInitializeError, name);
				}
				bind_functions[name](cell[name].asString());
			}
		}
		bind_functions.clear();
	}

	bool MetaData::OnLoad() 
	{
		return true;
	}
}