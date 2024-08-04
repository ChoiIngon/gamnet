#ifndef _META_DATA_H_
#define _META_DATA_H_

#include "CSVReader.h"
#include <functional>
#include <map>
#include <set>
#include <memory>
#include <string>
#include <Gamnet/Gamnet.h>
#include <Gamnet/Library/Time/DateTime.h>
#include <Gamnet/Library/Exception.h>

class Table
{
private :
	struct Header
	{
		std::string name;
		int index = -1;
		std::shared_ptr<Header> child;
	};

	struct Cell
	{
		std::string file_name;
		int row_num;
		std::shared_ptr<Header> header;
		std::string value;
	};

public :
	template <class Derived>
	class MetaData
	{
		struct MemberProperty
		{
			std::function<void(const std::shared_ptr<Cell>&)> init_func;
		};

	public:
		MetaData()
		{
			MetaData<Derived>::member_properties.clear();
		}

		void Init(const std::shared_ptr<Cell>& cell)
		{
			const std::string& key = cell->header->name;
			const std::string& value = cell->value;

			if ("" == value)
			{
				return;
			}

			auto itr = member_properties.find(key);
			if (member_properties.end() == itr)
			{
				return;
			}

			std::shared_ptr<MemberProperty> member_property = itr->second;

			try {
				member_property->init_func(cell);
			}
			catch (const boost::bad_lexical_cast&)
			{
				throw Gamnet::Exception(-1, "[MetaData] meta data load fail(meta_data:", cell->file_name, ", row_num:", cell->row_num, ", column:", key, ", reason:bed lexical cast)");
			}
			catch (const Gamnet::Exception& e)
			{
				throw e;
			}
		}

		virtual void OnLoad() {};

	protected:
		template <class T>
		void Bind(const std::string& name, T& member)
		{
			std::shared_ptr<MemberProperty> member_property = FindProperty(name);

			member_property->init_func = [this, &member](const std::shared_ptr<Cell>& cell) {
				this->Allocation(member, cell);
				};
		}

		template <class T>
		void Bind(const std::string& name, std::vector<T>& member)
		{
			std::shared_ptr<MemberProperty> member_property = FindProperty(name);

			member_property->init_func = [this, &member](const std::shared_ptr<Cell>& cell) {
				int index = cell->header->index;
				if ((int)member.size() <= index)
				{
					member.resize(index + 1);
				}
				T& elmt = member[index];
				Allocation(elmt, cell);
				};
		}

		template <class T, class F> requires std::is_member_function_pointer_v<F>
		void Bind(const std::string& name, T& member, F mem_func_ptr)
		{
			std::shared_ptr<MemberProperty> member_property = FindProperty(name);

			auto f = std::bind((void (MetaData::*)(T & member, const std::string&))mem_func_ptr, this, std::ref(member), std::placeholders::_1);

			member_property->init_func = [f](const std::shared_ptr<Cell>& cell) {
				const std::string& value = cell->value;
				if ("" == value)
				{
					return;
				}
				f(value);
				};
		}

	private:
		void Allocation(bool& member, const std::shared_ptr<Cell>& cell)
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
		void Allocation(std::string& member, const std::shared_ptr<Cell>& cell)
		{
			const std::string& value = cell->value;
			member = value;
		}
		void Allocation(Gamnet::Time::DateTime& member, const std::shared_ptr<Cell>& cell)
		{
			const std::string& value = cell->value;
			member = value;
		}
		template <class T> requires std::derived_from<T, MetaData<T>>
		void Allocation(T& member, const std::shared_ptr<Cell>& cell)
		{
			if (nullptr == cell->header->child)
			{
				return;
			}

			std::shared_ptr<Cell> child = std::make_shared<Cell>();
			child->file_name = cell->file_name;
			child->row_num = cell->row_num;
			child->header = cell->header->child;
			child->value = cell->value;

			member.Init(child);
		}
		template <class T>
		void Allocation(std::shared_ptr<T>& member, const std::shared_ptr<Cell>& cell)
		{
			if (nullptr == member)
			{
				member = std::make_shared<T>();
			}

			std::shared_ptr<Cell> child = std::make_shared<Cell>();
			child->file_name = cell->file_name;
			child->row_num = cell->row_num;
			child->header = cell->header->child;
			child->value = cell->value;

			member->Init(child);
		}
		template <class T>
		void Allocation(T& member, const std::shared_ptr<Cell>& cell)
		{
			const std::string& value = cell->value;
			if ("" == value)
			{
				return;
			}
			member = boost::lexical_cast<T>(value);
		}
	private:
		static std::shared_ptr<MemberProperty> FindProperty(const std::string& name)
		{
			auto itr = member_properties.find(name);
			if (member_properties.end() == itr)
			{
				std::shared_ptr<MemberProperty> member_property = std::make_shared<MemberProperty>();
				member_properties.insert(std::make_pair(name, member_property));
				return member_property;
			}
			return itr->second;
		}
		static std::map<std::string, std::shared_ptr<MemberProperty>>	member_properties;
	};

	template <class T>
	class MetaReader
	{
	public:
		typedef std::vector<std::shared_ptr<T>> MetaDatas;
		typedef std::vector<std::shared_ptr<T>>::iterator iterator;

		bool Read(const std::string& filePath)
		{
			meta_datas.clear();
			Table::table_name = filePath;
			Table::errors.clear();

			CSVReader csv;
			if (false == csv.ReadFile(filePath))
			{
				return false;
			}

			std::vector<std::shared_ptr<Header>> headers;
			for (const std::string& name : csv.GetColumnNames())
			{
				headers.push_back(ReadHeader(name));
			}
			
			
			int rowNum = 3; // 3라인 부터 데이터 시작

			for (auto itr : csv)
			{
				std::shared_ptr<T> meta = std::make_shared<T>();

				Table::row_num = rowNum;

				for (int i = 0; i < headers.size(); i++)
				{
					std::shared_ptr<Cell> cell = std::make_shared<Cell>();
					cell->file_name = filePath;
					cell->row_num = rowNum;
					cell->header = headers[i];
					cell->value = itr.GetValue(i);
					meta->Init(cell);
				}

				rowNum++;

				meta->OnLoad();
				meta_datas.push_back(meta);
			}

			if (0 < Table::errors.size())
			{
				for (const std::string& error : errors)
				{
					Gamnet::Log::Write(Gamnet::Log::Logger::LOG_LEVEL_ERR, "ERR ", error);
				}

				meta_datas.clear();
				return false;
			}
			return true;
		}

		const std::shared_ptr<T>& operator[](int rowNum)
		{
			return meta_datas[rowNum];
		}

		size_t Count()
		{
			return meta_datas.size();
		}

		iterator begin()
		{
			return meta_datas.begin();
		}

		iterator end()
		{
			return meta_datas.end();
		}

	private:
		std::shared_ptr<Header> ReadHeader(const std::string& cell)
		{
			std::shared_ptr<Header> header = std::make_shared<Header>();

			std::string headerName = cell;
			std::size_t dotPos = cell.find('.');
			if (std::string::npos != dotPos) // cellValue has '.'. it means this column is hierarchy
			{
				headerName = cell.substr(0, dotPos);
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
				header->child = ReadHeader(cell.substr(dotPos + 1));
			}
			return header;
		}

		MetaDatas meta_datas;
	};

	static void Assert(bool condition, const std::string& what)
	{
		if (true == condition)
		{
			return;
		}
		
		Table::errors.push_back(Gamnet::Format("Meta Table Assert(", what, ") - table:", table_name, ", row_num:", row_num));
	}
private :
	static std::string table_name;
	static std::string column_name;
	static int row_num;
	static std::list<std::string> errors;
};

template <class T>
std::map<std::string, std::shared_ptr<typename Table::MetaData<T>::MemberProperty>>	Table::MetaData<T>::member_properties;

#define META_MEMBER(member, ...) \
	Bind(#member, member, __VA_ARGS__)

#define META_ASSERT(condition) \
	Table::Assert(condition, #condition);

#endif
