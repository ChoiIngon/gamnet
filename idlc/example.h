#ifndef __example_H__
#define __example_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>

//#include "other_idl.h"

enum EnumTest {
		ENUM_TEST_1,
		ENUM_TEST_2,
}; // EnumTest
struct EnumTest_Serializer {
	static bool Store(char** _buf_, const EnumTest& obj) { 
		(*(EnumTest*)(*_buf_)) = obj;	(*_buf_) += sizeof(EnumTest);
		return true;
	}
	static bool Load(EnumTest& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(EnumTest) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(EnumTest));		(*_buf_) += sizeof(EnumTest); nSize -= sizeof(EnumTest);
		return true;
	}
	static int32_t Size(const EnumTest& obj) { return sizeof(EnumTest); }
};
enum EnumTest2 {
		ENUM_TEST2_1 = 100,
		ENUM_TEST2_2 = 0x0fffffff,
}; // EnumTest2
struct EnumTest2_Serializer {
	static bool Store(char** _buf_, const EnumTest2& obj) { 
		(*(EnumTest2*)(*_buf_)) = obj;	(*_buf_) += sizeof(EnumTest2);
		return true;
	}
	static bool Load(EnumTest2& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(EnumTest2) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(EnumTest2));		(*_buf_) += sizeof(EnumTest2); nSize -= sizeof(EnumTest2);
		return true;
	}
	static int32_t Size(const EnumTest2& obj) { return sizeof(EnumTest2); }
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
	static int32_t Size(const arr_i16_t& obj) { 
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
	static int32_t Size(const complex_arr_i16_t& obj) { 
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
	static int32_t Size(const lst_string_t& obj) { 
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
	static int32_t Size(const complex_lst_string_t& obj) { 
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
	static int32_t Size(const map_i32_string_t& obj) { 
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
	static int32_t Size(const complex_map_i32_string_t& obj) { 
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
struct DefaultData {
	char	byte_;
	unsigned char	ubyte_;
	bool	boolean_;
	int16_t	i16_;
	int32_t	i32_;
	int64_t	i64_;
	uint16_t	ui16_;
	uint32_t	ui32_;
	uint64_t	ui64_;
	double	double_;
	float	float_;
	std::string	string_;
	DefaultData()	{
		byte_ = '\0';
		ubyte_ = '\0';
		boolean_ = false;
		i16_ = 0;
		i32_ = 0;
		i64_ = 0;
		ui16_ = 0;
		ui32_ = 0;
		ui64_ = 0;
		double_ = 0.0;
		float_ = 0.0;
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += sizeof(char);
		nSize += sizeof(unsigned char);
		nSize += sizeof(bool);
		nSize += sizeof(int16_t);
		nSize += sizeof(int32_t);
		nSize += sizeof(int64_t);
		nSize += sizeof(uint16_t);
		nSize += sizeof(uint32_t);
		nSize += sizeof(uint64_t);
		nSize += sizeof(double);
		nSize += sizeof(float);
		nSize += sizeof(uint32_t); nSize += string_.length();
		return nSize;
	}
	bool Store(std::vector<char>& _buf_) const {
		size_t nSize = Size();
 		if(0 == nSize) { return true; }
		if(nSize > _buf_.size()) { 
			_buf_.resize(nSize);
		}
		char* pBuf = &(_buf_[0]);
		if(false == Store(&pBuf)) return false;
		return true;
	}
	bool Store(char** _buf_) const {
		std::memcpy(*_buf_, &byte_, sizeof(char)); (*_buf_) += sizeof(char);
		std::memcpy(*_buf_, &ubyte_, sizeof(unsigned char)); (*_buf_) += sizeof(unsigned char);
		std::memcpy(*_buf_, &boolean_, sizeof(bool)); (*_buf_) += sizeof(bool);
		std::memcpy(*_buf_, &i16_, sizeof(int16_t)); (*_buf_) += sizeof(int16_t);
		std::memcpy(*_buf_, &i32_, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, &i64_, sizeof(int64_t)); (*_buf_) += sizeof(int64_t);
		std::memcpy(*_buf_, &ui16_, sizeof(uint16_t)); (*_buf_) += sizeof(uint16_t);
		std::memcpy(*_buf_, &ui32_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &ui64_, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		std::memcpy(*_buf_, &double_, sizeof(double)); (*_buf_) += sizeof(double);
		std::memcpy(*_buf_, &float_, sizeof(float)); (*_buf_) += sizeof(float);
		size_t string__size = string_.length();
		std::memcpy(*_buf_, &string__size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, string_.c_str(), string_.length()); (*_buf_) += string_.length();
		return true;
	}
	bool Load(const std::vector<char>& _buf_) {
		size_t nSize = _buf_.size();
 		if(0 == nSize) { return true; }
		const char* pBuf = &(_buf_[0]);
		if(false == Load(&pBuf, nSize)) return false;
		return true;
	}
	bool Load(const char** _buf_, size_t& nSize) {
		if(sizeof(char) > nSize) { return false; }	std::memcpy(&byte_, *_buf_, sizeof(char));	(*_buf_) += sizeof(char); nSize -= sizeof(char);
		if(sizeof(unsigned char) > nSize) { return false; }	std::memcpy(&ubyte_, *_buf_, sizeof(unsigned char));	(*_buf_) += sizeof(unsigned char); nSize -= sizeof(unsigned char);
		if(sizeof(bool) > nSize) { return false; }	std::memcpy(&boolean_, *_buf_, sizeof(bool));	(*_buf_) += sizeof(bool); nSize -= sizeof(bool);
		if(sizeof(int16_t) > nSize) { return false; }	std::memcpy(&i16_, *_buf_, sizeof(int16_t));	(*_buf_) += sizeof(int16_t); nSize -= sizeof(int16_t);
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&i32_, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		if(sizeof(int64_t) > nSize) { return false; }	std::memcpy(&i64_, *_buf_, sizeof(int64_t));	(*_buf_) += sizeof(int64_t); nSize -= sizeof(int64_t);
		if(sizeof(uint16_t) > nSize) { return false; }	std::memcpy(&ui16_, *_buf_, sizeof(uint16_t));	(*_buf_) += sizeof(uint16_t); nSize -= sizeof(uint16_t);
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&ui32_, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&ui64_, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(double) > nSize) { return false; }	std::memcpy(&double_, *_buf_, sizeof(double));	(*_buf_) += sizeof(double); nSize -= sizeof(double);
		if(sizeof(float) > nSize) { return false; }	std::memcpy(&float_, *_buf_, sizeof(float));	(*_buf_) += sizeof(float); nSize -= sizeof(float);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t string__length = 0; std::memcpy(&string__length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < string__length) { return false; }
		string_.assign((char*)*_buf_, string__length); (*_buf_) += string__length; nSize -= string__length;
		return true;
	}
}; //DefaultData
struct DefaultData_Serializer {
	static bool Store(char** _buf_, const DefaultData& obj) { return obj.Store(_buf_); }
	static bool Load(DefaultData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const DefaultData& obj) { return obj.Size(); }
};
struct ContainerData {
	std::map<int32_t, std::string >	map_;
	map_i32_string_t	map_i32_string_;
	std::vector<std::string >	array_;
	std::string	static_array_[256];
	std::list<int32_t >	list_;
	ContainerData()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += sizeof(uint32_t);
		for(std::map<int32_t, std::string >::const_iterator map__itr = map_.begin(); map__itr != map_.end(); map__itr++) {
			const std::string& map__elmt = map__itr->second;
			nSize += sizeof(int32_t);
			nSize += sizeof(uint32_t); nSize += map__elmt.length();
		}
		nSize += map_i32_string_t_Serializer::Size(map_i32_string_);
		nSize += sizeof(int32_t);
		for(std::vector<std::string >::const_iterator array__itr = array_.begin(); array__itr != array_.end(); array__itr++)	{
			const std::string& array__elmt = *array__itr;
			nSize += sizeof(uint32_t); nSize += array__elmt.length();
		}
		for(int32_t i=0; i<256; i++) {
			nSize += sizeof(uint32_t); nSize += static_array_[i].length();
		}
		nSize += sizeof(int32_t);
		for(std::list<int32_t >::const_iterator list__itr = list_.begin(); list__itr != list_.end(); list__itr++)	{
			nSize += sizeof(int32_t);
		}
		return nSize;
	}
	bool Store(std::vector<char>& _buf_) const {
		size_t nSize = Size();
 		if(0 == nSize) { return true; }
		if(nSize > _buf_.size()) { 
			_buf_.resize(nSize);
		}
		char* pBuf = &(_buf_[0]);
		if(false == Store(&pBuf)) return false;
		return true;
	}
	bool Store(char** _buf_) const {
		size_t map__size = map_.size();
		std::memcpy(*_buf_, &map__size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::map<int32_t, std::string >::const_iterator map__itr = map_.begin(); map__itr != map_.end(); map__itr++) {
			const int32_t& map__key = map__itr->first;
			const std::string& map__elmt = map__itr->second;
			std::memcpy(*_buf_, &map__key, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			size_t map__elmt_size = map__elmt.length();
			std::memcpy(*_buf_, &map__elmt_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			std::memcpy(*_buf_, map__elmt.c_str(), map__elmt.length()); (*_buf_) += map__elmt.length();
		}
		if(false == map_i32_string_t_Serializer::Store(_buf_, map_i32_string_)) { return false; }
		size_t array__size = array_.size();
		std::memcpy(*_buf_, &array__size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::vector<std::string >::const_iterator array__itr = array_.begin(); array__itr != array_.end(); array__itr++)	{
			const std::string& array__elmt = *array__itr;
			size_t array__elmt_size = array__elmt.length();
			std::memcpy(*_buf_, &array__elmt_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			std::memcpy(*_buf_, array__elmt.c_str(), array__elmt.length()); (*_buf_) += array__elmt.length();
		}
		for(int32_t i=0; i<256; i++) {
			size_t static_array__i__size = static_array_[i].length();
			std::memcpy(*_buf_, &static_array__i__size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			std::memcpy(*_buf_, static_array_[i].c_str(), static_array_[i].length()); (*_buf_) += static_array_[i].length();
		}
		size_t list__size = list_.size();
		std::memcpy(*_buf_, &list__size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<int32_t >::const_iterator list__itr = list_.begin(); list__itr != list_.end(); list__itr++)	{
			const int32_t& list__elmt = *list__itr;
			std::memcpy(*_buf_, &list__elmt, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		}
		return true;
	}
	bool Load(const std::vector<char>& _buf_) {
		size_t nSize = _buf_.size();
 		if(0 == nSize) { return true; }
		const char* pBuf = &(_buf_[0]);
		if(false == Load(&pBuf, nSize)) return false;
		return true;
	}
	bool Load(const char** _buf_, size_t& nSize) {
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t map__length = 0; std::memcpy(&map__length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<map__length; i++) {
			int32_t map__key;
			if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&map__key, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
			std::string map__val;
			if(sizeof(int32_t) > nSize) { return false; }
			uint32_t map__val_length = 0; std::memcpy(&map__val_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
			if(nSize < map__val_length) { return false; }
			map__val.assign((char*)*_buf_, map__val_length); (*_buf_) += map__val_length; nSize -= map__val_length;
			map_.insert(std::make_pair(map__key, map__val));
		}
		if(false == map_i32_string_t_Serializer::Load(map_i32_string_, _buf_, nSize)) { return false; }
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t array__length = 0; std::memcpy(&array__length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<array__length; i++) {
			std::string array__val;
			if(sizeof(int32_t) > nSize) { return false; }
			uint32_t array__val_length = 0; std::memcpy(&array__val_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
			if(nSize < array__val_length) { return false; }
			array__val.assign((char*)*_buf_, array__val_length); (*_buf_) += array__val_length; nSize -= array__val_length;
			array_.push_back(array__val);
		}
		for(int32_t i=0; i<256; i++) {
			std::string static_array__val;
			if(sizeof(int32_t) > nSize) { return false; }
			uint32_t static_array__val_length = 0; std::memcpy(&static_array__val_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
			if(nSize < static_array__val_length) { return false; }
			static_array__val.assign((char*)*_buf_, static_array__val_length); (*_buf_) += static_array__val_length; nSize -= static_array__val_length;
			static_array_[i] = static_array__val;
		}
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t list__length = 0; std::memcpy(&list__length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<list__length; i++) {
			int32_t list__val;
			if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&list__val, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
			list_.push_back(list__val);
		}
		return true;
	}
}; //ContainerData
struct ContainerData_Serializer {
	static bool Store(char** _buf_, const ContainerData& obj) { return obj.Store(_buf_); }
	static bool Load(ContainerData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const ContainerData& obj) { return obj.Size(); }
};
struct DerivedData : public DefaultData {
	ContainerData	containerData_;
	DerivedData() : DefaultData()	{
	}
	DerivedData(const DefaultData& base) : DefaultData(base)	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += DefaultData::Size();
		nSize += ContainerData_Serializer::Size(containerData_);
		return nSize;
	}
	bool Store(std::vector<char>& _buf_) const {
		size_t nSize = Size();
 		if(0 == nSize) { return true; }
		if(nSize > _buf_.size()) { 
			_buf_.resize(nSize);
		}
		char* pBuf = &(_buf_[0]);
		if(false == Store(&pBuf)) return false;
		return true;
	}
	bool Store(char** _buf_) const {
		if(false == DefaultData::Store(_buf_)) return false;
		if(false == ContainerData_Serializer::Store(_buf_, containerData_)) { return false; }
		return true;
	}
	bool Load(const std::vector<char>& _buf_) {
		size_t nSize = _buf_.size();
 		if(0 == nSize) { return true; }
		const char* pBuf = &(_buf_[0]);
		if(false == Load(&pBuf, nSize)) return false;
		return true;
	}
	bool Load(const char** _buf_, size_t& nSize) {
		if(false == DefaultData::Load(_buf_, nSize)) return false;
		if(false == ContainerData_Serializer::Load(containerData_, _buf_, nSize)) { return false; }
		return true;
	}
}; //DerivedData
struct DerivedData_Serializer {
	static bool Store(char** _buf_, const DerivedData& obj) { return obj.Store(_buf_); }
	static bool Load(DerivedData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const DerivedData& obj) { return obj.Size(); }
};
struct MsgCliSvr_Category_MessageName_Req {
	enum { MSG_ID = 501 }; 
	MsgCliSvr_Category_MessageName_Req()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
		return nSize;
	}
	bool Store(std::vector<char>& _buf_) const {
		size_t nSize = Size();
 		if(0 == nSize) { return true; }
		if(nSize > _buf_.size()) { 
			_buf_.resize(nSize);
		}
		char* pBuf = &(_buf_[0]);
		if(false == Store(&pBuf)) return false;
		return true;
	}
	bool Store(char** _buf_) const {
		return true;
	}
	bool Load(const std::vector<char>& _buf_) {
		size_t nSize = _buf_.size();
 		if(0 == nSize) { return true; }
		const char* pBuf = &(_buf_[0]);
		if(false == Load(&pBuf, nSize)) return false;
		return true;
	}
	bool Load(const char** _buf_, size_t& nSize) {
		return true;
	}
}; //MsgCliSvr_Category_MessageName_Req
struct MsgCliSvr_Category_MessageName_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Category_MessageName_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Category_MessageName_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgCliSvr_Category_MessageName_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Category_MessageName_Ans {
	enum { MSG_ID = 502 }; 
	int32_t	nErrorCode;
	DerivedData	derivedData;
	MsgSvrCli_Category_MessageName_Ans()	{
		nErrorCode = 0;
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += sizeof(int32_t);
		nSize += DerivedData_Serializer::Size(derivedData);
		return nSize;
	}
	bool Store(std::vector<char>& _buf_) const {
		size_t nSize = Size();
 		if(0 == nSize) { return true; }
		if(nSize > _buf_.size()) { 
			_buf_.resize(nSize);
		}
		char* pBuf = &(_buf_[0]);
		if(false == Store(&pBuf)) return false;
		return true;
	}
	bool Store(char** _buf_) const {
		std::memcpy(*_buf_, &nErrorCode, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		if(false == DerivedData_Serializer::Store(_buf_, derivedData)) { return false; }
		return true;
	}
	bool Load(const std::vector<char>& _buf_) {
		size_t nSize = _buf_.size();
 		if(0 == nSize) { return true; }
		const char* pBuf = &(_buf_[0]);
		if(false == Load(&pBuf, nSize)) return false;
		return true;
	}
	bool Load(const char** _buf_, size_t& nSize) {
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&nErrorCode, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		if(false == DerivedData_Serializer::Load(derivedData, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_Category_MessageName_Ans
struct MsgSvrCli_Category_MessageName_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Category_MessageName_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Category_MessageName_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgSvrCli_Category_MessageName_Ans& obj) { return obj.Size(); }
};
#endif // __example_H__
