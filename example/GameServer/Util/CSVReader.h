#ifndef _CSV_READER_H_
#define _CSV_READER_H_

#include <map>
#include <vector>
#include <string>

class CSVReader
{
public :
	class iterator
	{
	public:
		iterator(const CSVReader& reader, int rowIndex = -1);

		iterator& operator * ();
		iterator& operator ++ ();
		iterator& operator ++ (int);
		iterator* operator -> ();
		bool operator != (const iterator& itr) const;
		bool operator == (const iterator& itr) const;

		const std::string& GetValue(const std::string& columnName);
		const std::string& GetValue(int index);
	private :
		const CSVReader& reader;
		int row_num;
	};

	typedef iterator Row;
public :
	bool ReadFile(const std::string& filePath);
	bool ReadStream(const std::string& stream);

	int GetIndex(const std::string& columnName) const;
	const std::vector<std::string>& GetColumnNames() const;

	const std::vector<std::string>& GetRow(size_t row_num) const;
	size_t GetRowCount() const;

	iterator begin();
	iterator end() const;
	iterator operator[](size_t row_num);
private :
	std::vector<std::string> column_names;
	std::vector<std::vector<std::string>> rows;
	std::map<std::string, int> name_to_index;
};

#endif

