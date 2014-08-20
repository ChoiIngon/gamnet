/*
 * ResultSet.h
 *
 *  Created on: Jul 25, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_RESULTSET_H_
#define GAMNET_DATABASE_RESULTSET_H_

#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <memory>
#include <boost/lexical_cast.hpp>

namespace Gamnet { namespace Database {
struct ResultSet
{
	int affectedRowCount_;
	int errno_;
	std::shared_ptr<sql::ResultSet> resultSet_;

	struct iterator
	{
		bool hasNext_;
		std::shared_ptr<sql::ResultSet> resultSet_;

		iterator();
		iterator(const iterator& itr);
		iterator& operator = (const iterator& itr);
		iterator& operator ++ (int);
		std::shared_ptr<sql::ResultSet> operator -> ();

		bool operator != (const ResultSet::iterator& itr) const;
		bool operator == (const ResultSet::iterator& itr) const;
		template <class T>
		const T GetValue(const std::string& column_name)
		{
			return boost::lexical_cast<T>(resultSet_->getString(column_name));
		}
	};

	ResultSet();
	virtual ~ResultSet();

	int GetSQLError() const;
	int GetAffectedRow() const;
	int GetRowCount();

	iterator begin();
	iterator end() const;
	iterator operator [] (int index);
};

}}
#endif /* RESULTSET_H_ */
