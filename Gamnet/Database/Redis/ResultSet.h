#ifndef _GAMNET_DATABASE_REDIS_RESULTSET_H_
#define _GAMNET_DATABASE_REDIS_RESULTSET_H_

#include <string>
#include <vector>
#include <memory>
#include "../../Library/Variant.h"

namespace Gamnet { namespace Database {	namespace Redis {
	class Connection;

	struct ResultSetImpl : public std::vector<Variant> {
		std::shared_ptr<Connection> conn_;

		std::string error;
	};

	struct ResultSet {
		std::shared_ptr<ResultSetImpl> impl_;

		typedef std::vector<Variant>::iterator iterator;

		ResultSet();
		iterator begin();
		iterator end() const;
		const Variant& operator[](unsigned int index);

		const std::string& error() const;
	};
} } }
#endif

