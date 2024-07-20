export module Gamnet.String;

import <string>;
import <sstream>;
import <iostream>;
import <concepts>;

namespace Gamnet 
{
	template <class T>
	void Concat(std::stringstream& stream, const T& t)
	{
		stream << t;
	}
	
	template <class T, class... ARGS>
	void Concat(std::stringstream& stream, const T& t, ARGS&... args)
	{
		stream << t;
		Concat(stream, args...);
	}
	
	export template <class... ARGS>
	std::string Format(ARGS&&... args)
	{
		std::stringstream stream;
		Concat(stream, args...);
		return stream.str();
	}

	template <class T> size_t UTF8Length(const T& str);

	template <class T>
	concept StringLike = requires(const T& t)
	{
		t.length();
		t[0];
	};
		
	export template <class T> requires StringLike<T>
	size_t UTF8Length(const T& str)
	{
		size_t utf8_char_count = 0;

		for (int i = 0; i < str.length(); )
		{
			// 4 바이트 문자인지 확인
			// 0xF0 = 1111 0000
			if (0xF0 == (0xF0 & str[i]))
			{
				// 나머지 3 바이트 확인
				// 0x80 = 1000 0000
				if (0x80 != (0x80 & str[i + 1]) || 0x80 != (0x80 & str[i + 2]) || 0x80 != (0x80 & str[i + 3]))
				{
					throw std::exception("not utf-8 encoded string");
				}

				i += 4;
				utf8_char_count++;
				continue;
			}
			// 3 바이트 문자인지 확인
			// 0xE0 = 1110 0000
			else if (0xE0 == (0xE0 & str[i]))
			{
				// 나머지 2 바이트 확인
				// 0x80 = 1000 0000
				if (0x80 != (0x80 & str[i + 1]) || 0x80 != (0x80 & str[i + 2]))
				{
					throw std::exception("not utf-8 encoded string");
				}

				i += 3;
				utf8_char_count++;
				continue;
			}
			// 2 바이트 문자인지 확인
			// 0xC0 = 1100 0000
			else if (0xC0 == (0xC0 & str[i]))
			{
				// 나머지 1 바이트 확인
				// 0x80 = 1000 0000
				if (0x80 != (0x80 & str[i + 1]))
				{
					throw std::exception("not utf-8 encoded string");
				}

				i += 2;
				utf8_char_count++;
				continue;
			}
			// 최상위 비트가 0인지 확인
			else if (0 == (str[i] >> 7))
			{
				i += 1;
				utf8_char_count++;
			}
			else
			{
				throw std::exception("not utf-8 encoded string");
			}
		}
		return utf8_char_count;
	}

	export template <class T> requires std::is_array_v<T>
	size_t UTF8Length(const T& str)
	{
		typedef std::remove_all_extents<T>::type elmt_type;
		typedef std::basic_string<elmt_type> string_type;
		return UTF8Length(string_type(str));
	}
}