#ifndef GAMNET_LIB_VARIANT_H
#define GAMNET_LIB_VARIANT_H

#include <string>
#include <boost/lexical_cast.hpp>

namespace Gamnet {
	class Variant {
	private:
		std::string value;
	public:
		Variant();
		Variant(const std::string& val);
		Variant(const Variant& val);

		operator bool() const;

		operator std::string();
		operator std::string() const;
		operator double() const;
		operator float() const;

		operator uint16_t() const;
		operator uint32_t()	const;
		operator uint64_t()	const;

		operator int16_t() const;
		operator int32_t() const;
		operator int64_t() const;

		const Variant& operator = (const Variant& rhs);
		const Variant& operator = (const std::string& rhs);
	};

} /* namespace Gamnet */

#endif
