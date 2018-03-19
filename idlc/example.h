#ifndef __example_H__
#define __example_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>

//#include "other_idl.h"

struct EnumTest {
	enum TYPE {
			ENUM_TEST_1,
			ENUM_TEST_2,
	};
	TYPE type;
	EnumTest() : type(ENUM_TEST_1) {}
	EnumTest(int obj) : type((TYPE)obj) {} 
	EnumTest(TYPE obj) : type(obj) {}
	operator TYPE() const { return type; }
	operator int() const { return (int)type; }
	EnumTest& operator = (const TYPE& type) { this->type = type; return *this; }
	bool operator == (const EnumTest& obj) { return type == obj.type; }
	bool operator == (EnumTest::TYPE type) { return this->type == type; }
	bool operator == (int value) { return this->type == value; }
	bool operator != (const EnumTest& obj) { return type != obj.type; }
	bool operator != (EnumTest::TYPE type) { return this->type != type; }
	bool operator != (int value) { return this->type != value; }
	bool operator < (const EnumTest& obj) { return type < obj.type; }
	bool operator < (EnumTest::TYPE type) { return this->type < type; }
	bool operator < (int value) { return this->type < value; }
	bool operator > (const EnumTest& obj) { return type > obj.type; }
	bool operator > (EnumTest::TYPE type) { return this->type > type; }
	bool operator > (int value) { return this->type > value; }
	bool operator <= (const EnumTest& obj) { return type <= obj.type; }
	bool operator <= (EnumTest::TYPE type) { return this->type <= type; }
	bool operator <= (int value) { return this->type <= value; }
	bool operator >= (const EnumTest& obj) { return type >= obj.type; }
	bool operator >= (EnumTest::TYPE type) { return this->type >= type; }
	bool operator >= (int value) { return this->type >= value; }
}; // EnumTest
struct EnumTest_Serializer {
	static bool Store(char** _buf_, const EnumTest& obj) { 
		(*(EnumTest::TYPE*)(*_buf_)) = obj.type;	(*_buf_) += sizeof(EnumTest::TYPE);
		return true;
	}
	static bool Load(EnumTest& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(EnumTest::TYPE) > nSize) { return false; }		std::memcpy(&obj.type, *_buf_, sizeof(EnumTest::TYPE));		(*_buf_) += sizeof(EnumTest::TYPE); nSize -= sizeof(EnumTest::TYPE);
		return true;
	}
	static size_t Size(const EnumTest& obj) { return sizeof(EnumTest::TYPE); }
};
struct EnumTest2 {
	enum TYPE {
			ENUM_TEST2_1 = 100,
			ENUM_TEST2_2 = 0x0fffffff,
	};
	TYPE type;
	EnumTest2() : type(ENUM_TEST2_1) {}
	EnumTest2(int obj) : type((TYPE)obj) {} 
	EnumTest2(TYPE obj) : type(obj) {}
	operator TYPE() const { return type; }
	operator int() const { return (int)type; }
	EnumTest2& operator = (const TYPE& type) { this->type = type; return *this; }
	bool operator == (const EnumTest2& obj) { return type == obj.type; }
	bool operator == (EnumTest2::TYPE type) { return this->type == type; }
	bool operator == (int value) { return this->type == value; }
	bool operator != (const EnumTest2& obj) { return type != obj.type; }
	bool operator != (EnumTest2::TYPE type) { return this->type != type; }
	bool operator != (int value) { return this->type != value; }
	bool operator < (const EnumTest2& obj) { return type < obj.type; }
	bool operator < (EnumTest2::TYPE type) { return this->type < type; }
	bool operator < (int value) { return this->type < value; }
	bool operator > (const EnumTest2& obj) { return type > obj.type; }
	bool operator > (EnumTest2::TYPE type) { return this->type > type; }
	bool operator > (int value) { return this->type > value; }
	bool operator <= (const EnumTest2& obj) { return type <= obj.type; }
	bool operator <= (EnumTest2::TYPE type) { return this->type <= type; }
	bool operator <= (int value) { return this->type <= value; }
	bool operator >= (const EnumTest2& obj) { return type >= obj.type; }
	bool operator >= (EnumTest2::TYPE type) { return this->type >= type; }
	bool operator >= (int value) { return this->type >= value; }
}; // EnumTest2
struct EnumTest2_Serializer {
	static bool Store(char** _buf_, const EnumTest2& obj) { 
		(*(EnumTest2::TYPE*)(*_buf_)) = obj.type;	(*_buf_) += sizeof(EnumTest2::TYPE);
		return true;
	}
	static bool Load(EnumTest2& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(EnumTest2::TYPE) > nSize) { return false; }		std::memcpy(&obj.type, *_buf_, sizeof(EnumTest2::TYPE));		(*_buf_) += sizeof(EnumTest2::TYPE); nSize -= sizeof(EnumTest2::TYPE);
		return true;
	}
	static size_t Size(const EnumTest2& obj) { return sizeof(EnumTest2::TYPE); }
};
typedef std::vector<int16_t > arr_i16_t;
struct arr_i16_t_Serializer {
	static bool Store(char** _buf_, const arr_i16_t& obj) { 
		size_t obj_size = obj.size();
		std::memcpy(*_buf_, &obj_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::vector<int16_t >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++)	{
			const int16_t& obj_elmt = *obj_itr;
			std::memcpy(*_buf_, &obj_elmt, sizeof(int16_t)); (*_buf_) += sizeof(int16_t);
		}
	return true;
	}
	static bool Load(arr_i16_t& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t obj_length = 0; std::memcpy(&obj_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<obj_length; i++) {
			int16_t obj_val;
			if(sizeof(int16_t) > nSize) { return false; }	std::memcpy(&obj_val, *_buf_, sizeof(int16_t));	(*_buf_) += sizeof(int16_t); nSize -= sizeof(int16_t);
			obj.push_back(obj_val);
		}
		return true;
	}
	static size_t Size(const arr_i16_t& obj) { 
		int32_t nSize = 0;
		nSize += sizeof(int32_t);
		for(std::vector<int16_t >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++)	{
			nSize += sizeof(int16_t);
		}
		return nSize;
	}
};
typedef std::vector<arr_i16_t > complex_arr_i16_t;
struct complex_arr_i16_t_Serializer {
	static bool Store(char** _buf_, const complex_arr_i16_t& obj) { 
		size_t obj_size = obj.size();
		std::memcpy(*_buf_, &obj_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::vector<arr_i16_t >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++)	{
			const arr_i16_t& obj_elmt = *obj_itr;
			if(false == arr_i16_t_Serializer::Store(_buf_, obj_elmt)) { return false; }
		}
	return true;
	}
	static bool Load(complex_arr_i16_t& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t obj_length = 0; std::memcpy(&obj_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<obj_length; i++) {
			arr_i16_t obj_val;
			if(false == arr_i16_t_Serializer::Load(obj_val, _buf_, nSize)) { return false; }
			obj.push_back(obj_val);
		}
		return true;
	}
	static size_t Size(const complex_arr_i16_t& obj) { 
		int32_t nSize = 0;
		nSize += sizeof(int32_t);
		for(std::vector<arr_i16_t >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++)	{
			const arr_i16_t& obj_elmt = *obj_itr;
			nSize += arr_i16_t_Serializer::Size(obj_elmt);
		}
		return nSize;
	}
};
typedef std::list<std::string > lst_string_t;
struct lst_string_t_Serializer {
	static bool Store(char** _buf_, const lst_string_t& obj) { 
		size_t obj_size = obj.size();
		std::memcpy(*_buf_, &obj_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<std::string >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++)	{
			const std::string& obj_elmt = *obj_itr;
			size_t obj_elmt_size = obj_elmt.length();
			std::memcpy(*_buf_, &obj_elmt_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			std::memcpy(*_buf_, obj_elmt.c_str(), obj_elmt.length()); (*_buf_) += obj_elmt.length();
		}
	return true;
	}
	static bool Load(lst_string_t& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t obj_length = 0; std::memcpy(&obj_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<obj_length; i++) {
			std::string obj_val;
			if(sizeof(int32_t) > nSize) { return false; }
			uint32_t obj_val_length = 0; std::memcpy(&obj_val_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
			if(nSize < obj_val_length) { return false; }
			obj_val.assign((char*)*_buf_, obj_val_length); (*_buf_) += obj_val_length; nSize -= obj_val_length;
			obj.push_back(obj_val);
		}
		return true;
	}
	static size_t Size(const lst_string_t& obj) { 
		int32_t nSize = 0;
		nSize += sizeof(int32_t);
		for(std::list<std::string >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++)	{
			const std::string& obj_elmt = *obj_itr;
			nSize += sizeof(uint32_t); nSize += obj_elmt.length();
		}
		return nSize;
	}
};
typedef std::list<lst_string_t > complex_lst_string_t;
struct complex_lst_string_t_Serializer {
	static bool Store(char** _buf_, const complex_lst_string_t& obj) { 
		size_t obj_size = obj.size();
		std::memcpy(*_buf_, &obj_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<lst_string_t >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++)	{
			const lst_string_t& obj_elmt = *obj_itr;
			if(false == lst_string_t_Serializer::Store(_buf_, obj_elmt)) { return false; }
		}
	return true;
	}
	static bool Load(complex_lst_string_t& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t obj_length = 0; std::memcpy(&obj_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<obj_length; i++) {
			lst_string_t obj_val;
			if(false == lst_string_t_Serializer::Load(obj_val, _buf_, nSize)) { return false; }
			obj.push_back(obj_val);
		}
		return true;
	}
	static size_t Size(const complex_lst_string_t& obj) { 
		int32_t nSize = 0;
		nSize += sizeof(int32_t);
		for(std::list<lst_string_t >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++)	{
			const lst_string_t& obj_elmt = *obj_itr;
			nSize += lst_string_t_Serializer::Size(obj_elmt);
		}
		return nSize;
	}
};
typedef std::map<int32_t, std::string > map_i32_string_t;
struct map_i32_string_t_Serializer {
	static bool Store(char** _buf_, const map_i32_string_t& obj) { 
		size_t obj_size = obj.size();
		std::memcpy(*_buf_, &obj_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::map<int32_t, std::string >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++) {
			const int32_t& obj_key = obj_itr->first;
			const std::string& obj_elmt = obj_itr->second;
			std::memcpy(*_buf_, &obj_key, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			size_t obj_elmt_size = obj_elmt.length();
			std::memcpy(*_buf_, &obj_elmt_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			std::memcpy(*_buf_, obj_elmt.c_str(), obj_elmt.length()); (*_buf_) += obj_elmt.length();
		}
	return true;
	}
	static bool Load(map_i32_string_t& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t obj_length = 0; std::memcpy(&obj_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<obj_length; i++) {
			int32_t obj_key;
			if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&obj_key, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
			std::string obj_val;
			if(sizeof(int32_t) > nSize) { return false; }
			uint32_t obj_val_length = 0; std::memcpy(&obj_val_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
			if(nSize < obj_val_length) { return false; }
			obj_val.assign((char*)*_buf_, obj_val_length); (*_buf_) += obj_val_length; nSize -= obj_val_length;
			obj.insert(std::make_pair(obj_key, obj_val));
		}
		return true;
	}
	static size_t Size(const map_i32_string_t& obj) { 
		int32_t nSize = 0;
		nSize += sizeof(uint32_t);
		for(std::map<int32_t, std::string >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++) {
			const std::string& obj_elmt = obj_itr->second;
			nSize += sizeof(int32_t);
			nSize += sizeof(uint32_t); nSize += obj_elmt.length();
		}
		return nSize;
	}
};
typedef std::map<std::string, map_i32_string_t > complex_map_i32_string_t;
struct complex_map_i32_string_t_Serializer {
	static bool Store(char** _buf_, const complex_map_i32_string_t& obj) { 
		size_t obj_size = obj.size();
		std::memcpy(*_buf_, &obj_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::map<std::string, map_i32_string_t >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++) {
			const std::string& obj_key = obj_itr->first;
			const map_i32_string_t& obj_elmt = obj_itr->second;
			size_t obj_key_size = obj_key.length();
			std::memcpy(*_buf_, &obj_key_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			std::memcpy(*_buf_, obj_key.c_str(), obj_key.length()); (*_buf_) += obj_key.length();
			if(false == map_i32_string_t_Serializer::Store(_buf_, obj_elmt)) { return false; }
		}
	return true;
	}
	static bool Load(complex_map_i32_string_t& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t obj_length = 0; std::memcpy(&obj_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<obj_length; i++) {
			std::string obj_key;
			if(sizeof(int32_t) > nSize) { return false; }
			uint32_t obj_key_length = 0; std::memcpy(&obj_key_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
			if(nSize < obj_key_length) { return false; }
			obj_key.assign((char*)*_buf_, obj_key_length); (*_buf_) += obj_key_length; nSize -= obj_key_length;
			map_i32_string_t obj_val;
			if(false == map_i32_string_t_Serializer::Load(obj_val, _buf_, nSize)) { return false; }
			obj.insert(std::make_pair(obj_key, obj_val));
		}
		return true;
	}
	static size_t Size(const complex_map_i32_string_t& obj) { 
		int32_t nSize = 0;
		nSize += sizeof(uint32_t);
		for(std::map<std::string, map_i32_string_t >::const_iterator obj_itr = obj.begin(); obj_itr != obj.end(); obj_itr++) {
			const std::string& obj_key = obj_itr->first;
			const map_i32_string_t& obj_elmt = obj_itr->second;
			nSize += sizeof(uint32_t); nSize += obj_key.length();
			nSize += map_i32_string_t_Serializer::Size(obj_elmt);
		}
		return nSize;
	}
};
