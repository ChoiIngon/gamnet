#include "CSVReader.h"
#include <boost/algorithm/string.hpp>  
#include <fstream>
#include <string>
#include <streambuf>
#include <Gamnet/Log/Log.h>

CSVReader::iterator::iterator(const CSVReader& reader, int rowIndex)
	: reader(reader)
	, row_index(rowIndex)
{
}

const std::string& CSVReader::iterator::GetValue(const std::string& columnName)
{
	return GetValue(reader.GetIndex(boost::algorithm::to_lower_copy(columnName)));
}

const std::string& CSVReader::iterator::GetValue(int index)
{
	const std::vector<std::string>& row = reader.GetRow(row_index);
	if(0 > index || row.size() <= index)
	{
		throw std::out_of_range("invalid csv column index:" + std::to_string(index));
	}
	return row[index];
}

CSVReader::iterator& CSVReader::iterator::operator * ()
{
	return *this;
}

CSVReader::iterator& CSVReader::iterator::operator ++ ()
{
	row_index++;
	return *this;
}

CSVReader::iterator& CSVReader::iterator::operator ++ (int)
{
	row_index++;
	return *this;
}

CSVReader::iterator* CSVReader::iterator::operator -> ()
{
	return this;
}

bool CSVReader::iterator::operator != (const CSVReader::iterator& itr) const
{
	if(&reader != &itr.reader || row_index != itr.row_index)
	{
		return true;
	}
	return false;
}

bool CSVReader::iterator::operator == (const CSVReader::iterator& itr) const
{
	if (&reader == &itr.reader && row_index == itr.row_index)
	{
		return true;
	}

	return false;
}

bool CSVReader::ReadFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (true == file.fail())
	{
		LOG(ERR, "failed to open file(path:", filePath, ")");
		return false;
	}

	std::string stream((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return ReadStream(stream);
	
}
bool CSVReader::ReadStream(const std::string& str)
{
	std::stringstream stream(str);
	std::string line;
	std::string cell;
	// read column
	{	
		std::getline(stream, line);
		// check BOM format
		if (3 <= line.size() && (char)0xEF == line[0] && (char)0xBB == line[1] && (char)0xBF == line[2]) 
		{
			line = line.substr(3);
		}

		std::stringstream lineStream(line);
		while (std::getline(lineStream, cell, ','))
		{
			boost::algorithm::to_lower(cell);
			column_names.push_back(cell);
		}
	}
	
	// read data type
	{
		std::getline(stream, line);
	}

	// read data rows
	{
		std::vector<std::string> row;;
		std::streampos pos = stream.tellg();
		int quotes = 0;
		char prev = 0;

		std::string cell;
		for(size_t i = pos; i<str.size(); i++)
		{
			switch(str[i])
			{
			case '"' :
				++quotes;
				break;
			case ',' :
			case '\n' :
				if(0 == quotes || ('"' == prev && 0 == quotes % 2))
				{
					if(2 <= quotes)
					{
						cell = cell.substr(1);
						cell = cell.substr(0, cell.size() - 1);
					}
					if(2 < quotes)
					{
						std::string::size_type findPos = 0;
						std::string::size_type offset = 0;
						const std::string pattern("\"\"");
						const std::string replace("\"");
						while((findPos = cell.find(pattern, offset)) != std::string::npos)
						{
							cell.replace(cell.begin() + findPos, cell.begin() + findPos + pattern.size(), "\"");
							offset = findPos + replace.size();
						}
					}
					row.push_back(cell);
					cell = "";
					prev = 0;
					quotes = 0;
					if ('\n' == str[i])
					{
						rows.push_back(row);
						row.clear();
					}
					continue;
				}
				break;
			default :
				break;
			}
			cell += prev = str[i];
		}
	}

	return true;
}

int CSVReader::GetIndex(const std::string& columnName) const
{
	auto itr = name_to_index.find(columnName);
	if(name_to_index.end() == itr)
	{
		return -1;
	}
	return itr->second;
}

const std::vector<std::string>& CSVReader::GetColumnNames() const
{
	return column_names;
}

size_t CSVReader::GetRowCount() const
{
	return rows.size();
}

const std::vector<std::string>& CSVReader::GetRow(size_t rowIndex) const
{
	if(0 > rowIndex || rows.size() <= rowIndex)
	{
		throw std::out_of_range("out of range row num:" + std::to_string(rowIndex));
	}
	return rows[rowIndex];
}

CSVReader::iterator CSVReader::begin()
{
	return iterator(*this, 0);
}

CSVReader::iterator CSVReader::end() const
{
	return iterator(*this, rows.size());
}

CSVReader::iterator CSVReader::operator [] (size_t index)
{
	return iterator(*this, index);
}

