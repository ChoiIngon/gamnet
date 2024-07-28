#include "MetaData.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>  
#include <idl/MessageCommon.h>

void MetaData::Bind(const std::string& name, bool& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, int16_t& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, uint16_t& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, int32_t& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, uint32_t& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, int64_t& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, uint64_t& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, float& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, double& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, std::string& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Bind(const std::string& name, Gamnet::Time::DateTime& member)
{
	bind_functions.insert(std::make_pair(name, [this, &member](const std::shared_ptr<Cell>& cell) {
		this->Allocation(member, cell);
	}));
}

void MetaData::Init(const std::shared_ptr<Row>& row)
{
	for(auto cell : row->cells)
	{
		const std::string& key = cell->header->name;
		const std::string& value = cell->value;
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
		catch (const boost::bad_lexical_cast&)
		{
			throw Gamnet::Exception((int)Message::ErrorCode::UndefineError, "[MetaData] meta data load fail(meta_data:", cell->meta_name, ", row_num:", cell->row_num, ", column:", key, ", reason:bed lexical cast)");
		}
		catch (const Gamnet::Exception& e)
		{
			throw e;
		}
	}
}

void MetaData::OnLoad() 
{
}

void MetaData::Allocation(bool& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
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

void MetaData::Allocation(int16_t& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = boost::lexical_cast<int16_t>(value);
}

void MetaData::Allocation(uint16_t& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = boost::lexical_cast<uint16_t>(value);
}

void MetaData::Allocation(int32_t& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = boost::lexical_cast<int32_t>(value);
}

void MetaData::Allocation(uint32_t& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = boost::lexical_cast<uint32_t>(value);
}

void MetaData::Allocation(int64_t& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = boost::lexical_cast<int64_t>(value);
}

void MetaData::Allocation(uint64_t& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = boost::lexical_cast<uint64_t>(value);
}

void MetaData::Allocation(float& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = boost::lexical_cast<float>(value);
}

void MetaData::Allocation(double& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = boost::lexical_cast<double>(value);
}

void MetaData::Allocation(std::string& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = value;
}

void MetaData::Allocation(Gamnet::Time::DateTime& member, const std::shared_ptr<Cell>& cell)
{
	const std::string& value = cell->value;
	member = value;
}

void MetaData::Allocation(MetaData& member, const std::shared_ptr<Cell>& cell)
{
	std::shared_ptr<Row> row = std::make_shared<Row>();
	std::shared_ptr<Cell> child = std::make_shared<Cell>();
	child->meta_name = cell->meta_name;
	child->row_num = cell->row_num;
	child->header = cell->header->child;
	child->value = cell->value;
	row->cells.push_back(child);

	member.Init(row);
}
