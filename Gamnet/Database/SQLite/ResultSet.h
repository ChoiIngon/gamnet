#ifndef GAMNET_DATABASE_SQLITE_RESULTSET_H_
#define GAMNET_DATABASE_SQLITE_RESULTSET_H_

#include <memory>
#include <map>
#include <vector>
#include "../../Library/Variant.h"

namespace Gamnet { namespace Database { namespace SQLite {
	class Connection;

	struct ResultSetImpl 
	{
		std::shared_ptr<Connection> conn_;

		std::map<std::string, unsigned short> mapColumnName_;
		std::vector<std::vector<std::string>> vecRows_;
		uint32_t rowCount_;

		ResultSetImpl();
		~ResultSetImpl();
	};

	struct ResultSet 
	{
		std::shared_ptr<ResultSetImpl> impl_;
		struct iterator
		{
			std::vector<std::vector<std::string>>::iterator itr_;
			std::shared_ptr<ResultSetImpl> impl_;

			iterator();
			iterator(const std::shared_ptr<ResultSetImpl>& impl);

			iterator& operator ++ (int);
			iterator* operator -> ();

			bool operator != (const ResultSet::iterator& itr) const;
			bool operator == (const ResultSet::iterator& itr) const;

			Variant operator[] (const std::string& name);
		};

		ResultSet();
		virtual ~ResultSet();

		unsigned int GetRowCount() const;

		iterator begin();
		iterator end() const;
		iterator operator [] (unsigned int row_num);
	};
}}}
#endif
