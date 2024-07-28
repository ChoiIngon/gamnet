#ifndef _META_DATA_H_
#define _META_DATA_H_

#include <functional>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <Gamnet/Library/Time/DateTime.h>
#include <Gamnet/Library/Variant.h>
#include <Gamnet/Library/Exception.h>
#include "CSVReader.h"

class MetaData
{
public :
	struct ColumnHeader
	{
		std::string name;
		int index = -1;
		std::shared_ptr<ColumnHeader> child;
	};

	struct Cell
	{
		std::shared_ptr<ColumnHeader> header;
		std::string value;
	};

	struct Row
	{
		std::string file;
		int row_num;
		std::vector<std::shared_ptr<Cell>> cells;
	};

public:
	void Init(const std::shared_ptr<Row>& row);
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
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const std::shared_ptr<Cell>& cell) {
			this->Allocation(member, cell);
		}));
	}
	template <class T>
	void Bind(const std::string& name, std::vector<T>& member)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [this, &member](const std::shared_ptr<Cell>& cell) {
			if ("" == cell->value)
			{
				return;
			}
			int index = cell->header->index;
			if((int)member.size() <= index)
			{
				member.resize(index + 1);
			}
			T& elmt = member[index];
			Allocation(elmt, cell);
		}));
	}
	/*
		F = void(T& member, const std::string& value)
	*/
	template <class F>
	void CustomBind(const std::string& name, F f)
	{
		bind_functions.insert(std::make_pair(boost::algorithm::to_lower_copy(name), [f](const std::shared_ptr<Cell>& cell) {
			const std::string& value = cell->value;
			if ("" == value)
			{
				return;
			}
			f(value);
		}));
	}
private :
	void Allocation(bool& member, const std::shared_ptr<Cell>& cell);
	void Allocation(int16_t& member, const std::shared_ptr<Cell>& cell);
	void Allocation(uint16_t& member, const std::shared_ptr<Cell>& cell);
	void Allocation(int32_t& member, const std::shared_ptr<Cell>& cell);
	void Allocation(uint32_t& member, const std::shared_ptr<Cell>& cell);
	void Allocation(int64_t& member, const std::shared_ptr<Cell>& cell);
	void Allocation(uint64_t& member, const std::shared_ptr<Cell>& cell);
	void Allocation(float& member, const std::shared_ptr<Cell>& cell);
	void Allocation(double& member, const std::shared_ptr<Cell>& cell);
	void Allocation(std::string& member, const std::shared_ptr<Cell>& cell);
	void Allocation(Gamnet::Time::DateTime& member, const std::shared_ptr<Cell>& cell);
	void Allocation(MetaData& member, const std::shared_ptr<Cell>& cell);
	template <class T>
	void Allocation(std::shared_ptr<T>& member, const std::shared_ptr<Cell>& cell)
	{
		std::shared_ptr<Row> row = std::make_shared<Row>();
		std::shared_ptr<Cell> child = std::make_shared<Cell>();
		child->header = cell->header->child;
		child->value = cell->value;
		row->cells.push_back(child);

		if (nullptr == member)
		{
			member = std::make_shared<T>();
		}

		member->Init(row);
	}

private :
	std::map<std::string, std::function<void(const std::shared_ptr<Cell>&)>>	bind_functions;
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

		std::vector<std::shared_ptr<MetaData::ColumnHeader>> headers;
		// read column
		for(const std::string& cell : csv.GetColumnNames())
		{
			headers.push_back(ReadColumnHeader(cell));
		}
		
		// read data rows
		int rowNum = 1;
		for(auto itr : csv)
		{
			std::shared_ptr<MetaData::Row> row = std::make_shared<MetaData::Row>();

			row->file = filePath;
			row->row_num = rowNum;

			bool emptyRow = true;
			for(int i=0; i<headers.size(); i++)
			{
				if ("" != itr.GetValue(i))
				{
					emptyRow = false;
				}

				std::shared_ptr<MetaData::Cell> cell = std::make_shared<MetaData::Cell>();
				cell->header = headers[i];
				cell->value = itr.GetValue(i);
				row->cells.push_back(cell);
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

	std::shared_ptr<MetaData::ColumnHeader> ReadColumnHeader(const std::string& cell)
	{
		std::shared_ptr<MetaData::ColumnHeader> header = std::make_shared<MetaData::ColumnHeader>();

		const std::string cellValue = boost::algorithm::to_lower_copy(cell);
		std::string headerName = cellValue; 
		std::size_t dotPos = cellValue.find('.');
		if (std::string::npos != dotPos) // cellValue has '.'. it means this column is hierarchy
		{
			headerName = cellValue.substr(0, dotPos);
		}

		std::size_t braceStartPos = headerName.find('[');
		std::size_t braceEndPos = headerName.find(']');
		if (std::string::npos != braceStartPos && std::string::npos != braceEndPos) // column has '[' and  ']'. it means column is array
		{
			header->index = boost::lexical_cast<int>(headerName.substr(braceStartPos + 1, braceEndPos - braceStartPos - 1));
		}

		std::size_t columnEndPos = std::min(dotPos, braceStartPos);
		header->name = headerName.substr(0, columnEndPos);
		if (std::string::npos != dotPos)
		{
			header->child = ReadColumnHeader(cellValue.substr(dotPos + 1));
		}
		return header;
	}

private :
	MetaDatas meta_datas;
};

#define META_MEMBER(member) \
	Bind(#member, member)

#define META_CUSTOM(member, func_ptr) \
	CustomBind(#member, std::bind(&func_ptr, this, std::ref(member), std::placeholders::_1))

#endif
