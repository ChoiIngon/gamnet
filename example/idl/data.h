#ifndef __data_H__
#define __data_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
struct ErrorCode {
	enum TYPE {
			Success = 0,
			MessageFormatError = 1000,
			MessageSeqOmmitError = 1001,
			InvalidUserError = 2000,
			InvalidAccessTokenError = 2001,
			DuplicateConnectionError = 3000,
			ReconnectTimeoutError = 4000,
			ResponseTimeoutError = 4001,
			AlreadyLoginSessionError = 5000,
	};
	TYPE type;
	ErrorCode() : type(Success) {}
	ErrorCode(int obj) : type((TYPE)obj) {} 
	ErrorCode(TYPE obj) : type(obj) {}
	operator TYPE() const { return type; }
	operator int() const { return (int)type; }
	ErrorCode& operator = (const TYPE& type) { this->type = type; return *this; }
	bool operator == (const ErrorCode& obj) { return type == obj.type; }
	bool operator == (ErrorCode::TYPE type) { return this->type == type; }
	bool operator == (int value) { return this->type == value; }
	bool operator != (const ErrorCode& obj) { return type != obj.type; }
	bool operator != (ErrorCode::TYPE type) { return this->type != type; }
	bool operator != (int value) { return this->type != value; }
	bool operator < (const ErrorCode& obj) { return type < obj.type; }
	bool operator < (ErrorCode::TYPE type) { return this->type < type; }
	bool operator < (int value) { return this->type < value; }
	bool operator > (const ErrorCode& obj) { return type > obj.type; }
	bool operator > (ErrorCode::TYPE type) { return this->type > type; }
	bool operator > (int value) { return this->type > value; }
	bool operator <= (const ErrorCode& obj) { return type <= obj.type; }
	bool operator <= (ErrorCode::TYPE type) { return this->type <= type; }
	bool operator <= (int value) { return this->type <= value; }
	bool operator >= (const ErrorCode& obj) { return type >= obj.type; }
	bool operator >= (ErrorCode::TYPE type) { return this->type >= type; }
	bool operator >= (int value) { return this->type >= value; }
}; // ErrorCode
struct ErrorCode_Serializer {
	static bool Store(char** _buf_, const ErrorCode& obj) { 
		(*(ErrorCode::TYPE*)(*_buf_)) = obj.type;	(*_buf_) += sizeof(ErrorCode::TYPE);
		return true;
	}
	static bool Load(ErrorCode& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(ErrorCode::TYPE) > nSize) { return false; }		std::memcpy(&obj.type, *_buf_, sizeof(ErrorCode::TYPE));		(*_buf_) += sizeof(ErrorCode::TYPE); nSize -= sizeof(ErrorCode::TYPE);
		return true;
	}
	static size_t Size(const ErrorCode& obj) { return sizeof(ErrorCode::TYPE); }
};
struct ItemType {
	enum TYPE {
			Invalid,
			Weapon,
			Armor,
			Potion,
	};
	TYPE type;
	ItemType() : type(Invalid) {}
	ItemType(int obj) : type((TYPE)obj) {} 
	ItemType(TYPE obj) : type(obj) {}
	operator TYPE() const { return type; }
	operator int() const { return (int)type; }
	ItemType& operator = (const TYPE& type) { this->type = type; return *this; }
	bool operator == (const ItemType& obj) { return type == obj.type; }
	bool operator == (ItemType::TYPE type) { return this->type == type; }
	bool operator == (int value) { return this->type == value; }
	bool operator != (const ItemType& obj) { return type != obj.type; }
	bool operator != (ItemType::TYPE type) { return this->type != type; }
	bool operator != (int value) { return this->type != value; }
	bool operator < (const ItemType& obj) { return type < obj.type; }
	bool operator < (ItemType::TYPE type) { return this->type < type; }
	bool operator < (int value) { return this->type < value; }
	bool operator > (const ItemType& obj) { return type > obj.type; }
	bool operator > (ItemType::TYPE type) { return this->type > type; }
	bool operator > (int value) { return this->type > value; }
	bool operator <= (const ItemType& obj) { return type <= obj.type; }
	bool operator <= (ItemType::TYPE type) { return this->type <= type; }
	bool operator <= (int value) { return this->type <= value; }
	bool operator >= (const ItemType& obj) { return type >= obj.type; }
	bool operator >= (ItemType::TYPE type) { return this->type >= type; }
	bool operator >= (int value) { return this->type >= value; }
}; // ItemType
struct ItemType_Serializer {
	static bool Store(char** _buf_, const ItemType& obj) { 
		(*(ItemType::TYPE*)(*_buf_)) = obj.type;	(*_buf_) += sizeof(ItemType::TYPE);
		return true;
	}
	static bool Load(ItemType& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(ItemType::TYPE) > nSize) { return false; }		std::memcpy(&obj.type, *_buf_, sizeof(ItemType::TYPE));		(*_buf_) += sizeof(ItemType::TYPE); nSize -= sizeof(ItemType::TYPE);
		return true;
	}
	static size_t Size(const ItemType& obj) { return sizeof(ItemType::TYPE); }
};
struct ItemData {
	std::string	item_id;
	ItemType	item_type;
	uint32_t	item_seq;
	ItemData()	{
		item_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += item_id.length();
		nSize += ItemType_Serializer::Size(item_type);
		nSize += sizeof(uint32_t);
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
		size_t item_id_size = item_id.length();
		std::memcpy(*_buf_, &item_id_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, item_id.c_str(), item_id.length()); (*_buf_) += item_id.length();
		if(false == ItemType_Serializer::Store(_buf_, item_type)) { return false; }
		std::memcpy(*_buf_, &item_seq, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		uint32_t item_id_length = 0; std::memcpy(&item_id_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < item_id_length) { return false; }
		item_id.assign((char*)*_buf_, item_id_length); (*_buf_) += item_id_length; nSize -= item_id_length;
		if(false == ItemType_Serializer::Load(item_type, _buf_, nSize)) { return false; }
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&item_seq, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //ItemData
struct ItemData_Serializer {
	static bool Store(char** _buf_, const ItemData& obj) { return obj.Store(_buf_); }
	static bool Load(ItemData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const ItemData& obj) { return obj.Size(); }
};
struct UserData {
	std::string	user_id;
	uint32_t	user_seq;
	std::list<ItemData >	items;
	UserData()	{
		user_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += user_id.length();
		nSize += sizeof(uint32_t);
		nSize += sizeof(int32_t);
		for(std::list<ItemData >::const_iterator items_itr = items.begin(); items_itr != items.end(); items_itr++)	{
			const ItemData& items_elmt = *items_itr;
			nSize += ItemData_Serializer::Size(items_elmt);
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
		size_t user_id_size = user_id.length();
		std::memcpy(*_buf_, &user_id_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, user_id.c_str(), user_id.length()); (*_buf_) += user_id.length();
		std::memcpy(*_buf_, &user_seq, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		size_t items_size = items.size();
		std::memcpy(*_buf_, &items_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<ItemData >::const_iterator items_itr = items.begin(); items_itr != items.end(); items_itr++)	{
			const ItemData& items_elmt = *items_itr;
			if(false == ItemData_Serializer::Store(_buf_, items_elmt)) { return false; }
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
		uint32_t user_id_length = 0; std::memcpy(&user_id_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < user_id_length) { return false; }
		user_id.assign((char*)*_buf_, user_id_length); (*_buf_) += user_id_length; nSize -= user_id_length;
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&user_seq, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t items_length = 0; std::memcpy(&items_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<items_length; i++) {
			ItemData items_val;
			if(false == ItemData_Serializer::Load(items_val, _buf_, nSize)) { return false; }
			items.push_back(items_val);
		}
		return true;
	}
}; //UserData
struct UserData_Serializer {
	static bool Store(char** _buf_, const UserData& obj) { return obj.Store(_buf_); }
	static bool Load(UserData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const UserData& obj) { return obj.Size(); }
};
#endif // __data_H__
