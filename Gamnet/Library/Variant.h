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
		Variant(const Variant& val);
		Variant(const std::string& val);
		Variant(const char* val);
		Variant(bool val);
		Variant(double val);
		Variant(float val);
		Variant(uint16_t val);
		Variant(uint32_t val);
		Variant(uint64_t val);
		Variant(int16_t val);
		Variant(int32_t val);
		Variant(int64_t val);
		
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
		const Variant& operator = (bool rhs);
		const Variant& operator = (double rhs);
		const Variant& operator = (float rhs);
		const Variant& operator = (uint16_t rhs);
		const Variant& operator = (uint32_t rhs);
		const Variant& operator = (uint64_t rhs);
		const Variant& operator = (int16_t rhs);
		const Variant& operator = (int32_t rhs);
		const Variant& operator = (int64_t rhs);

		bool operator < (const Variant& rhs);
		bool operator == (const Variant& rhs);
		bool operator != (const Variant& rhs);
	};

	
	
} /* namespace Gamnet */

#endif
