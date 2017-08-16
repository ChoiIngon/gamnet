#ifndef __message_H__
#define __message_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>

// 
//
//

enum ERROR_CODE {
		ERROR_SUCCESS = 0,
		ERROR_INVALID_MSG_FORMAT = 1000,
		ERROR_INVALID_USER = 2000,
		ERROR_DUPLICATE_CONNECTION = 3000,
		ERROR_CANT_FIND_CACHE_DATA = 4000,
		ERROR_INVALID_ACCESSTOKEN = 5000,
		ERROR_INCORRECT_DATA = 6000,
}; // ERROR_CODE
struct ERROR_CODE_Serializer {
	static bool Store(char** _buf_, const ERROR_CODE& obj) { 
		(*(ERROR_CODE*)(*_buf_)) = obj;	(*_buf_) += sizeof(ERROR_CODE);
		return true;
	}
	static bool Load(ERROR_CODE& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(ERROR_CODE) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(ERROR_CODE));		(*_buf_) += sizeof(ERROR_CODE); nSize -= sizeof(ERROR_CODE);
		return true;
	}
	static int32_t Size(const ERROR_CODE& obj) { return sizeof(ERROR_CODE); }
};
struct ItemData {
	std::string	item_id;
	uint32_t	item_seq;
	ItemData()	{
		item_seq = 0;
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += item_id.length();
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&item_seq, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //ItemData
struct ItemData_Serializer {
	static bool Store(char** _buf_, const ItemData& obj) { return obj.Store(_buf_); }
	static bool Load(ItemData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const ItemData& obj) { return obj.Size(); }
};
struct UserData {
	std::string	user_id;
	uint32_t	user_seq;
	std::string	access_token;
	uint32_t	msg_seq;
	uint64_t	kickout_time;
	uint64_t	session_key;
	std::list<ItemData >	items;
	UserData()	{
		user_seq = 0;
		msg_seq = 0;
		kickout_time = 0;
		session_key = 0;
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += user_id.length();
		nSize += sizeof(uint32_t);
		nSize += sizeof(uint32_t); nSize += access_token.length();
		nSize += sizeof(uint32_t);
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint64_t);
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
		size_t access_token_size = access_token.length();
		std::memcpy(*_buf_, &access_token_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, access_token.c_str(), access_token.length()); (*_buf_) += access_token.length();
		std::memcpy(*_buf_, &msg_seq, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &kickout_time, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		std::memcpy(*_buf_, &session_key, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
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
		uint32_t access_token_length = 0; std::memcpy(&access_token_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < access_token_length) { return false; }
		access_token.assign((char*)*_buf_, access_token_length); (*_buf_) += access_token_length; nSize -= access_token_length;
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&msg_seq, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&kickout_time, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&session_key, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
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
	static int32_t Size(const UserData& obj) { return obj.Size(); }
};
struct MsgCliSvr_Login_Req {
	enum { MSG_ID = 10000001 }; 
	std::string	user_id;
	std::string	access_token;
	MsgCliSvr_Login_Req()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += user_id.length();
		nSize += sizeof(uint32_t); nSize += access_token.length();
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
		size_t access_token_size = access_token.length();
		std::memcpy(*_buf_, &access_token_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, access_token.c_str(), access_token.length()); (*_buf_) += access_token.length();
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
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t access_token_length = 0; std::memcpy(&access_token_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < access_token_length) { return false; }
		access_token.assign((char*)*_buf_, access_token_length); (*_buf_) += access_token_length; nSize -= access_token_length;
		return true;
	}
}; //MsgCliSvr_Login_Req
struct MsgCliSvr_Login_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Login_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Login_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgCliSvr_Login_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Login_Ans {
	enum { MSG_ID = 10000001 }; 
	ERROR_CODE	error_code;
	UserData	user_data;
	MsgSvrCli_Login_Ans()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += ERROR_CODE_Serializer::Size(error_code);
		nSize += UserData_Serializer::Size(user_data);
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
		if(false == ERROR_CODE_Serializer::Store(_buf_, error_code)) { return false; }
		if(false == UserData_Serializer::Store(_buf_, user_data)) { return false; }
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
		if(false == ERROR_CODE_Serializer::Load(error_code, _buf_, nSize)) { return false; }
		if(false == UserData_Serializer::Load(user_data, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_Login_Ans
struct MsgSvrCli_Login_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Login_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Login_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgSvrCli_Login_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_Kickout_Ntf {
	enum { MSG_ID = 10000002 }; 
	ERROR_CODE	error_code;
	MsgSvrCli_Kickout_Ntf()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += ERROR_CODE_Serializer::Size(error_code);
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
		if(false == ERROR_CODE_Serializer::Store(_buf_, error_code)) { return false; }
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
		if(false == ERROR_CODE_Serializer::Load(error_code, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_Kickout_Ntf
struct MsgSvrCli_Kickout_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Kickout_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Kickout_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgSvrCli_Kickout_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_HeartBeat_Ntf {
	enum { MSG_ID = 10000003 }; 
	uint32_t	msg_seq;
	MsgCliSvr_HeartBeat_Ntf()	{
		msg_seq = 0;
	}
	int32_t Size() const {
		int32_t nSize = 0;
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
		std::memcpy(*_buf_, &msg_seq, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&msg_seq, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgCliSvr_HeartBeat_Ntf
struct MsgCliSvr_HeartBeat_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_HeartBeat_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_HeartBeat_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgCliSvr_HeartBeat_Ntf& obj) { return obj.Size(); }
};
#endif // __message_H__
