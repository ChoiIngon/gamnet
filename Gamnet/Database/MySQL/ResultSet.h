/*
 * ResultSet.h
 *
 *  Created on: Jul 25, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_MYSQL_RESULTSET_H_
#define GAMNET_DATABASE_MYSQL_RESULTSET_H_

#include <mysql.h>
#include <memory>
#include <map>
#include "../../Library/Variant.h"
#include "../../Library/Json/json.h"

namespace Gamnet { namespace Database { namespace MySQL {
	class Connection;

	class ResultSetImpl
	{
	public :
		ResultSetImpl(const std::shared_ptr<Connection> conn);
		~ResultSetImpl();

		bool StoreResult();
		bool NextResult();

		MYSQL_RES* mysql_res;
		uint64_t affected_rows;
		uint64_t last_insert_id;
		unsigned int error_code;
		std::map<std::string, unsigned short> columns;
	private :
		const std::shared_ptr<Connection> conn;
	};

	struct ResultSet
	{
		struct iterator
		{
			std::shared_ptr<ResultSetImpl> impl_;

			MYSQL_ROW row_;

			iterator();
			iterator(const std::shared_ptr<ResultSetImpl>& impl);

			iterator& operator * ();
			iterator& operator ++ ();
			iterator& operator ++ (int);
			iterator* operator -> ();

			bool operator != (const ResultSet::iterator& itr) const;
			bool operator == (const ResultSet::iterator& itr) const;
			
			const std::string getString(const std::string& column_name) const;
			uint32_t getUInt(const std::string& column_name) const;
			uint16_t getUInt16(const std::string& column_name) const;
			uint32_t getUInt32(const std::string& column_name) const;
			uint64_t getUInt64(const std::string& column_name) const;
			int32_t getInt(const std::string& column_name) const;
			int16_t getInt16(const std::string& column_name) const;
			int32_t getInt32(const std::string& column_name) const;
			int64_t getInt64(const std::string& column_name) const;
			bool getBool(const std::string& column_name) const;
			float getFloat(const std::string& column_name) const;
			double getDouble(const std::string& column_name) const;
			Variant operator [] (const std::string& column_name) const;
		};

		ResultSet(const std::shared_ptr<ResultSetImpl>& impl);
		virtual ~ResultSet();

		unsigned int GetAffectedRow() const;
		unsigned int GetRowCount();
		unsigned int GetLastInsertID() const;
		unsigned int GetLastError() const;
		bool NextResult();
		Json::Value ToJson();

		iterator begin();
		iterator end() const;
		iterator operator [] (unsigned int index);

		ResultSet& operator = (const std::shared_ptr<ResultSetImpl>& impl);
	private :
		std::shared_ptr<ResultSetImpl> impl_;
	};

} } }
#endif /* RESULTSET_H_ */
