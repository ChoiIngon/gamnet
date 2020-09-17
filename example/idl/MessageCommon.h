#ifndef __MessageCommon_H__
#define __MessageCommon_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
#include <stdexcept>

namespace Message { 

enum class ErrorCode {
	Success = 0,
	MessageFormatError = 1000,
	MessageSeqOmmitError = 1001,
	InvalidUserError = 2000,
	InvalidAccessTokenError = 2001,
	InvalidSeviceName = 2002,
	DuplicateConnectionError = 3000,
	ReconnectTimeoutError = 4000,
	ResponseTimeoutError = 4001,
	AlreadyLoginSessionError = 5000,
	CanNotCreateCastGroup = 5001,
	DuplicateNameError,
	CreateAccountError,
	UndefineError = 99999,
}; // ErrorCode
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<ErrorCode>(ErrorCode e) { 
	static const std::map<ErrorCode, std::string> table = {
		{ ErrorCode::Success, "Success"},
		{ ErrorCode::MessageFormatError, "MessageFormatError"},
		{ ErrorCode::MessageSeqOmmitError, "MessageSeqOmmitError"},
		{ ErrorCode::InvalidUserError, "InvalidUserError"},
		{ ErrorCode::InvalidAccessTokenError, "InvalidAccessTokenError"},
		{ ErrorCode::InvalidSeviceName, "InvalidSeviceName"},
		{ ErrorCode::DuplicateConnectionError, "DuplicateConnectionError"},
		{ ErrorCode::ReconnectTimeoutError, "ReconnectTimeoutError"},
		{ ErrorCode::ResponseTimeoutError, "ResponseTimeoutError"},
		{ ErrorCode::AlreadyLoginSessionError, "AlreadyLoginSessionError"},
		{ ErrorCode::CanNotCreateCastGroup, "CanNotCreateCastGroup"},
		{ ErrorCode::DuplicateNameError, "DuplicateNameError"},
		{ ErrorCode::CreateAccountError, "CreateAccountError"},
		{ ErrorCode::UndefineError, "UndefineError"},
	};
	auto itr = table.find(e); 
	if(table.end() == itr) { throw std::runtime_error("ToString<ErrorCode>()"); }
	return itr->second;
}
template<class T> T Parse(const std::string&);
template <> inline ErrorCode Parse<ErrorCode>(const std::string& s) {
	static const std::map<std::string, ErrorCode> table = {
		{ "Success", ErrorCode::Success},
		{ "MessageFormatError", ErrorCode::MessageFormatError},
		{ "MessageSeqOmmitError", ErrorCode::MessageSeqOmmitError},
		{ "InvalidUserError", ErrorCode::InvalidUserError},
		{ "InvalidAccessTokenError", ErrorCode::InvalidAccessTokenError},
		{ "InvalidSeviceName", ErrorCode::InvalidSeviceName},
		{ "DuplicateConnectionError", ErrorCode::DuplicateConnectionError},
		{ "ReconnectTimeoutError", ErrorCode::ReconnectTimeoutError},
		{ "ResponseTimeoutError", ErrorCode::ResponseTimeoutError},
		{ "AlreadyLoginSessionError", ErrorCode::AlreadyLoginSessionError},
		{ "CanNotCreateCastGroup", ErrorCode::CanNotCreateCastGroup},
		{ "DuplicateNameError", ErrorCode::DuplicateNameError},
		{ "CreateAccountError", ErrorCode::CreateAccountError},
		{ "UndefineError", ErrorCode::UndefineError},
	};
	auto itr = table.find(s); 
	if(table.end() == itr) { throw std::runtime_error("Parse<ErrorCode>()"); }
	return itr->second;
}
struct ErrorCode_Serializer {
	static bool Store(char** _buf_, const ErrorCode& obj) { 
		(*(ErrorCode*)(*_buf_)) = obj;	(*_buf_) += sizeof(ErrorCode);
		return true;
	}
	static bool Load(ErrorCode& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(ErrorCode) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(ErrorCode));		(*_buf_) += sizeof(ErrorCode); nSize -= sizeof(ErrorCode);
		return true;
	}
	static size_t Size(const ErrorCode& obj) { return sizeof(ErrorCode); }
};
enum class AccountType {
	Invalid,
	Dev = 1,
	Google = 2,
	Apple = 3,
	Facebook = 4,
}; // AccountType
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<AccountType>(AccountType e) { 
	static const std::map<AccountType, std::string> table = {
		{ AccountType::Invalid, "Invalid"},
		{ AccountType::Dev, "Dev"},
		{ AccountType::Google, "Google"},
		{ AccountType::Apple, "Apple"},
		{ AccountType::Facebook, "Facebook"},
	};
	auto itr = table.find(e); 
	if(table.end() == itr) { throw std::runtime_error("ToString<AccountType>()"); }
	return itr->second;
}
template<class T> T Parse(const std::string&);
template <> inline AccountType Parse<AccountType>(const std::string& s) {
	static const std::map<std::string, AccountType> table = {
		{ "Invalid", AccountType::Invalid},
		{ "Dev", AccountType::Dev},
		{ "Google", AccountType::Google},
		{ "Apple", AccountType::Apple},
		{ "Facebook", AccountType::Facebook},
	};
	auto itr = table.find(s); 
	if(table.end() == itr) { throw std::runtime_error("Parse<AccountType>()"); }
	return itr->second;
}
struct AccountType_Serializer {
	static bool Store(char** _buf_, const AccountType& obj) { 
		(*(AccountType*)(*_buf_)) = obj;	(*_buf_) += sizeof(AccountType);
		return true;
	}
	static bool Load(AccountType& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(AccountType) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(AccountType));		(*_buf_) += sizeof(AccountType); nSize -= sizeof(AccountType);
		return true;
	}
	static size_t Size(const AccountType& obj) { return sizeof(AccountType); }
};
enum class AccountState {
	Invalid,
	Normal,
	Penalty,
	OnDelete,
	Deleted,
}; // AccountState
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<AccountState>(AccountState e) { 
	static const std::map<AccountState, std::string> table = {
		{ AccountState::Invalid, "Invalid"},
		{ AccountState::Normal, "Normal"},
		{ AccountState::Penalty, "Penalty"},
		{ AccountState::OnDelete, "OnDelete"},
		{ AccountState::Deleted, "Deleted"},
	};
	auto itr = table.find(e); 
	if(table.end() == itr) { throw std::runtime_error("ToString<AccountState>()"); }
	return itr->second;
}
template<class T> T Parse(const std::string&);
template <> inline AccountState Parse<AccountState>(const std::string& s) {
	static const std::map<std::string, AccountState> table = {
		{ "Invalid", AccountState::Invalid},
		{ "Normal", AccountState::Normal},
		{ "Penalty", AccountState::Penalty},
		{ "OnDelete", AccountState::OnDelete},
		{ "Deleted", AccountState::Deleted},
	};
	auto itr = table.find(s); 
	if(table.end() == itr) { throw std::runtime_error("Parse<AccountState>()"); }
	return itr->second;
}
struct AccountState_Serializer {
	static bool Store(char** _buf_, const AccountState& obj) { 
		(*(AccountState*)(*_buf_)) = obj;	(*_buf_) += sizeof(AccountState);
		return true;
	}
	static bool Load(AccountState& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(AccountState) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(AccountState));		(*_buf_) += sizeof(AccountState); nSize -= sizeof(AccountState);
		return true;
	}
	static size_t Size(const AccountState& obj) { return sizeof(AccountState); }
};
enum class CounterType {
	Invalid = 0,
	Gold = 1,
	Cash = 2,
}; // CounterType
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<CounterType>(CounterType e) { 
	static const std::map<CounterType, std::string> table = {
		{ CounterType::Invalid, "Invalid"},
		{ CounterType::Gold, "Gold"},
		{ CounterType::Cash, "Cash"},
	};
	auto itr = table.find(e); 
	if(table.end() == itr) { throw std::runtime_error("ToString<CounterType>()"); }
	return itr->second;
}
template<class T> T Parse(const std::string&);
template <> inline CounterType Parse<CounterType>(const std::string& s) {
	static const std::map<std::string, CounterType> table = {
		{ "Invalid", CounterType::Invalid},
		{ "Gold", CounterType::Gold},
		{ "Cash", CounterType::Cash},
	};
	auto itr = table.find(s); 
	if(table.end() == itr) { throw std::runtime_error("Parse<CounterType>()"); }
	return itr->second;
}
struct CounterType_Serializer {
	static bool Store(char** _buf_, const CounterType& obj) { 
		(*(CounterType*)(*_buf_)) = obj;	(*_buf_) += sizeof(CounterType);
		return true;
	}
	static bool Load(CounterType& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(CounterType) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(CounterType));		(*_buf_) += sizeof(CounterType); nSize -= sizeof(CounterType);
		return true;
	}
	static size_t Size(const CounterType& obj) { return sizeof(CounterType); }
};
enum class ItemType {
	Invalid,
	Equip,
	Package,
}; // ItemType
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<ItemType>(ItemType e) { 
	static const std::map<ItemType, std::string> table = {
		{ ItemType::Invalid, "Invalid"},
		{ ItemType::Equip, "Equip"},
		{ ItemType::Package, "Package"},
	};
	auto itr = table.find(e); 
	if(table.end() == itr) { throw std::runtime_error("ToString<ItemType>()"); }
	return itr->second;
}
template<class T> T Parse(const std::string&);
template <> inline ItemType Parse<ItemType>(const std::string& s) {
	static const std::map<std::string, ItemType> table = {
		{ "Invalid", ItemType::Invalid},
		{ "Equip", ItemType::Equip},
		{ "Package", ItemType::Package},
	};
	auto itr = table.find(s); 
	if(table.end() == itr) { throw std::runtime_error("Parse<ItemType>()"); }
	return itr->second;
}
struct ItemType_Serializer {
	static bool Store(char** _buf_, const ItemType& obj) { 
		(*(ItemType*)(*_buf_)) = obj;	(*_buf_) += sizeof(ItemType);
		return true;
	}
	static bool Load(ItemType& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(ItemType) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(ItemType));		(*_buf_) += sizeof(ItemType); nSize -= sizeof(ItemType);
		return true;
	}
	static size_t Size(const ItemType& obj) { return sizeof(ItemType); }
};
enum class DungeonTileType {
	Invalid,
	Floor,
	Wall,
	Door,
	StairDown,
	StairUp,
}; // DungeonTileType
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<DungeonTileType>(DungeonTileType e) { 
	static const std::map<DungeonTileType, std::string> table = {
		{ DungeonTileType::Invalid, "Invalid"},
		{ DungeonTileType::Floor, "Floor"},
		{ DungeonTileType::Wall, "Wall"},
		{ DungeonTileType::Door, "Door"},
		{ DungeonTileType::StairDown, "StairDown"},
		{ DungeonTileType::StairUp, "StairUp"},
	};
	auto itr = table.find(e); 
	if(table.end() == itr) { throw std::runtime_error("ToString<DungeonTileType>()"); }
	return itr->second;
}
template<class T> T Parse(const std::string&);
template <> inline DungeonTileType Parse<DungeonTileType>(const std::string& s) {
	static const std::map<std::string, DungeonTileType> table = {
		{ "Invalid", DungeonTileType::Invalid},
		{ "Floor", DungeonTileType::Floor},
		{ "Wall", DungeonTileType::Wall},
		{ "Door", DungeonTileType::Door},
		{ "StairDown", DungeonTileType::StairDown},
		{ "StairUp", DungeonTileType::StairUp},
	};
	auto itr = table.find(s); 
	if(table.end() == itr) { throw std::runtime_error("Parse<DungeonTileType>()"); }
	return itr->second;
}
struct DungeonTileType_Serializer {
	static bool Store(char** _buf_, const DungeonTileType& obj) { 
		(*(DungeonTileType*)(*_buf_)) = obj;	(*_buf_) += sizeof(DungeonTileType);
		return true;
	}
	static bool Load(DungeonTileType& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(DungeonTileType) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(DungeonTileType));		(*_buf_) += sizeof(DungeonTileType); nSize -= sizeof(DungeonTileType);
		return true;
	}
	static size_t Size(const DungeonTileType& obj) { return sizeof(DungeonTileType); }
};
struct UserData {
	uint64_t	user_seq;
	std::string	user_name;
	UserData()	{
		user_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint32_t); nSize += user_name.length();
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
		std::memcpy(*_buf_, &user_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		size_t user_name_size = user_name.length();
		std::memcpy(*_buf_, &user_name_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, user_name.c_str(), user_name.length()); (*_buf_) += user_name.length();
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&user_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t user_name_length = 0; std::memcpy(&user_name_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < user_name_length) { return false; }
		user_name.assign((char*)*_buf_, user_name_length); (*_buf_) += user_name_length; nSize -= user_name_length;
		return true;
	}
}; //UserData
struct UserData_Serializer {
	static bool Store(char** _buf_, const UserData& obj) { return obj.Store(_buf_); }
	static bool Load(UserData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const UserData& obj) { return obj.Size(); }
};
struct MailData {
	uint64_t	mail_seq;
	std::string	mail_message;
	uint64_t	expire_date;
	uint32_t	item_id;
	uint32_t	item_count;
	MailData()	{
		mail_seq = 0;
		expire_date = 0;
		item_id = 0;
		item_count = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint32_t); nSize += mail_message.length();
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint32_t);
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
		std::memcpy(*_buf_, &mail_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		size_t mail_message_size = mail_message.length();
		std::memcpy(*_buf_, &mail_message_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, mail_message.c_str(), mail_message.length()); (*_buf_) += mail_message.length();
		std::memcpy(*_buf_, &expire_date, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		std::memcpy(*_buf_, &item_id, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &item_count, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&mail_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t mail_message_length = 0; std::memcpy(&mail_message_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < mail_message_length) { return false; }
		mail_message.assign((char*)*_buf_, mail_message_length); (*_buf_) += mail_message_length; nSize -= mail_message_length;
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&expire_date, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&item_id, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&item_count, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MailData
struct MailData_Serializer {
	static bool Store(char** _buf_, const MailData& obj) { return obj.Store(_buf_); }
	static bool Load(MailData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MailData& obj) { return obj.Size(); }
};
struct CounterData {
	CounterType	counter_id;
	uint64_t	update_date;
	int32_t	count;
	CounterData()	{
		update_date = 0;
		count = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += CounterType_Serializer::Size(counter_id);
		nSize += sizeof(uint64_t);
		nSize += sizeof(int32_t);
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
		if(false == CounterType_Serializer::Store(_buf_, counter_id)) { return false; }
		std::memcpy(*_buf_, &update_date, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		std::memcpy(*_buf_, &count, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		if(false == CounterType_Serializer::Load(counter_id, _buf_, nSize)) { return false; }
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&update_date, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&count, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		return true;
	}
}; //CounterData
struct CounterData_Serializer {
	static bool Store(char** _buf_, const CounterData& obj) { return obj.Store(_buf_); }
	static bool Load(CounterData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const CounterData& obj) { return obj.Size(); }
};
struct ItemData {
	uint64_t	item_seq;
	uint32_t	item_id;
	ItemType	item_type;
	int32_t	item_count;
	ItemData()	{
		item_seq = 0;
		item_id = 0;
		item_count = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint32_t);
		nSize += ItemType_Serializer::Size(item_type);
		nSize += sizeof(int32_t);
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
		std::memcpy(*_buf_, &item_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		std::memcpy(*_buf_, &item_id, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		if(false == ItemType_Serializer::Store(_buf_, item_type)) { return false; }
		std::memcpy(*_buf_, &item_count, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&item_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&item_id, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(false == ItemType_Serializer::Load(item_type, _buf_, nSize)) { return false; }
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&item_count, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		return true;
	}
}; //ItemData
struct ItemData_Serializer {
	static bool Store(char** _buf_, const ItemData& obj) { return obj.Store(_buf_); }
	static bool Load(ItemData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const ItemData& obj) { return obj.Size(); }
};
struct Vector2Int {
	int32_t	x;
	int32_t	y;
	Vector2Int()	{
		x = 0;
		y = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int32_t);
		nSize += sizeof(int32_t);
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
		std::memcpy(*_buf_, &x, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, &y, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&x, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&y, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		return true;
	}
}; //Vector2Int
struct Vector2Int_Serializer {
	static bool Store(char** _buf_, const Vector2Int& obj) { return obj.Store(_buf_); }
	static bool Load(Vector2Int& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const Vector2Int& obj) { return obj.Size(); }
};

}

#endif // __MessageCommon_H__
