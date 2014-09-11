/*
 * ResultSet.h
 *
 *  Created on: Jul 25, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_RESULTSET_H_
#define GAMNET_DATABASE_RESULTSET_H_

#include <mysql/mysql.h>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>

namespace Gamnet { namespace Database {

class Connection;

struct ResultSetImpl
{
	MYSQL_RES* res_;
	unsigned int affectedRowCount_;
	std::map<std::string, unsigned short> mapColumnName_;
	std::shared_ptr<Connection> conn_;

	ResultSetImpl();
	~ResultSetImpl();
};

struct ResultSet
{
	std::shared_ptr<ResultSetImpl> impl_;

	struct iterator
	{
		std::shared_ptr<ResultSetImpl> impl_;
		MYSQL_ROW row_;

		iterator();
		iterator(const std::shared_ptr<ResultSetImpl>& impl);

		iterator& operator ++ (int);
		iterator* operator -> ();

		bool operator != (const ResultSet::iterator& itr) const;
		bool operator == (const ResultSet::iterator& itr) const;
		template <class T>
		const T GetValue(const std::string& column_name)
		{
			return boost::lexical_cast<T>(getString(column_name));
		}
		const std::string getString(const std::string& column_name);

		uint32_t getUInt(const std::string& column_name)
		{
			return boost::lexical_cast<uint32_t>(getString(column_name));
		}
		uint16_t getUInt16(const std::string& column_name)
		{
			return boost::lexical_cast<uint16_t>(getString(column_name));
		}
		uint32_t getUInt32(const std::string& column_name)
		{
			return boost::lexical_cast<uint32_t>(getString(column_name));
		}
		uint64_t getUInt64(const std::string& column_name)
		{
			return boost::lexical_cast<uint64_t>(getString(column_name));
		}
		int32_t getInt(const std::string& column_name)
		{
			return boost::lexical_cast<int32_t>(getString(column_name));
		}
		int16_t getInt16(const std::string& column_name)
		{
			return boost::lexical_cast<int16_t>(getString(column_name));
		}
		int32_t getInt32(const std::string& column_name)
		{
			return boost::lexical_cast<int32_t>(getString(column_name));
		}
		int64_t getInt64(const std::string& column_name)
		{
			return boost::lexical_cast<int64_t>(getString(column_name));
		}
		bool getBoolean(const std::string& column_name)
		{
			return boost::lexical_cast<bool>(getString(column_name));
		}
		float getFloat(const std::string& column_name)
		{
			return boost::lexical_cast<float>(getString(column_name));
		}
		double getDouble(const std::string& column_name)
		{
			return boost::lexical_cast<double>(getString(column_name));
		}
	};

	ResultSet();
	virtual ~ResultSet();

	unsigned int GetAffectedRow() const;
	unsigned int GetRowCount();

	iterator begin();
	iterator end() const;
	iterator operator [] (unsigned int index);
};

}}
#endif /* RESULTSET_H_ */
