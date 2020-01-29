#ifndef _GAMNET_DATABASE_REDIS_RESULTSET_H_
#define _GAMNET_DATABASE_REDIS_RESULTSET_H_

#include <string>
#include <vector>
#include <memory>
#include "../../Library/Variant.h"
#include "../../Library/Json/json.h"

namespace Gamnet { namespace Database {	namespace Redis {
	class Connection;

	struct ResultSetImpl : public Json::Value//public std::vector<Variant> 
	{
	private :
		std::shared_ptr<Connection> conn_;
		size_t read_index;
		bool ParseString(Json::Value& output, const std::string& input);
		bool ParseInt(int& output, const std::string& input);
		bool ParseBulkString(Json::Value& output, const std::string& input);
		bool ParseArray(Json::Value& output, const std::string& input);
		bool Parse(Json::Value& result, const std::string& input);
	public :
		ResultSetImpl();
		ResultSetImpl(const std::shared_ptr<Connection> conn);
		std::string error;
		bool Parse(const std::string& input);
	};

	struct ResultSet 
	{
		std::shared_ptr<ResultSetImpl> impl_;

		typedef Json::Value::iterator iterator;

		ResultSet();
		iterator begin();
		iterator end() const;
		const Json::Value& operator[](unsigned int index);
		// std::shared_ptr<ResultSetImpl> operator -> ();
		Json::ArrayIndex size() const;
		bool empty() const;
		Json::Value value();

		// const std::string& error() const;
	};
} } }
#endif

