/*
 * ResultSet.h
 *
 *  Created on: Jul 25, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_MYSQL_RESULTSET_H_
#define GAMNET_DATABASE_MYSQL_RESULTSET_H_

#ifdef _WIN32
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif
#include <memory>
#include <map>
#include "../../Library/Variant.h"

namespace Gamnet { namespace Database { namespace MySQL {
	class Connection;

	struct ResultSetImpl
	{
		std::shared_ptr<Connection> conn_;

		MYSQL_RES* res_;
		unsigned int affectedRowCount_;
		unsigned int lastInsertID_;
		std::map<std::string, unsigned short> mapColumnName_;
		

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
			
			const std::string getString(const std::string& column_name);
			uint32_t getUInt(const std::string& column_name);
			uint16_t getUInt16(const std::string& column_name);
			uint32_t getUInt32(const std::string& column_name);
			uint64_t getUInt64(const std::string& column_name);
			int32_t getInt(const std::string& column_name);
			int16_t getInt16(const std::string& column_name);
			int32_t getInt32(const std::string& column_name);
			int64_t getInt64(const std::string& column_name);
			bool getBool(const std::string& column_name);
			float getFloat(const std::string& column_name);
			double getDouble(const std::string& column_name);
			Variant operator [] (const std::string& column_name);
		};

		ResultSet();
		virtual ~ResultSet();

		unsigned int GetAffectedRow() const;
		unsigned int GetRowCount();
		unsigned int GetLastInsertID() const;

		iterator begin();
		iterator end() const;
		iterator operator [] (unsigned int index);
	};

} } }
#endif /* RESULTSET_H_ */
