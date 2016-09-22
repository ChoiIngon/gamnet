#ifndef GAMNET_DATABASE_COLUMNVALUE_H_
#define GAMNET_DATABASE_COLUMNVALUE_H_

#include <string>
#include <boost/lexical_cast.hpp>

namespace Gamnet {	namespace Database {
	class ColumnValue {
	private:
		const std::string value;
	public:
		ColumnValue(const std::string& val);

		operator bool() const;
		operator const char*() const;
		operator std::string() const;

		operator double() const;
		operator float() const;
		
		operator uint16_t() const;
		operator uint32_t()	const;
		operator uint64_t()	const;

		operator int16_t() const;
		operator int32_t() const;
		operator int64_t() const;
	};

	} /* namespace Database */
} /* namespace Gamnet */

#endif /* GAMNET_DATABASE_COLUMNVALUE_H_ */
