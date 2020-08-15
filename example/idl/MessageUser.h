#ifndef __MessageUser_H__
#define __MessageUser_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>

#include "MessageCommon.h"
namespace Handler { namespace User {

struct MsgCliSvr_Login_Req {
	enum { MSG_ID = 11 }; 
	std::string	account_id;
	AccountType	account_type;
	MsgCliSvr_Login_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += account_id.length();
		nSize += AccountType_Serializer::Size(account_type);
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
		size_t account_id_size = account_id.length();
		std::memcpy(*_buf_, &account_id_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, account_id.c_str(), account_id.length()); (*_buf_) += account_id.length();
		if(false == AccountType_Serializer::Store(_buf_, account_type)) { return false; }
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
		uint32_t account_id_length = 0; std::memcpy(&account_id_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < account_id_length) { return false; }
		account_id.assign((char*)*_buf_, account_id_length); (*_buf_) += account_id_length; nSize -= account_id_length;
		if(false == AccountType_Serializer::Load(account_type, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgCliSvr_Login_Req
struct MsgCliSvr_Login_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Login_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Login_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_Login_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Login_Ans {
	enum { MSG_ID = 12 }; 
	ErrorCode	error_code;
	UserData	user_data;
	MsgSvrCli_Login_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(error_code);
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
		if(false == ErrorCode_Serializer::Store(_buf_, error_code)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(error_code, _buf_, nSize)) { return false; }
		if(false == UserData_Serializer::Load(user_data, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_Login_Ans
struct MsgSvrCli_Login_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Login_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Login_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Login_Ans& obj) { return obj.Size(); }
};
struct MsgCliSvr_Create_Req {
	enum { MSG_ID = 21 }; 
	std::string	account_id;
	AccountType	account_type;
	std::string	user_name;
	MsgCliSvr_Create_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += account_id.length();
		nSize += AccountType_Serializer::Size(account_type);
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
		size_t account_id_size = account_id.length();
		std::memcpy(*_buf_, &account_id_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, account_id.c_str(), account_id.length()); (*_buf_) += account_id.length();
		if(false == AccountType_Serializer::Store(_buf_, account_type)) { return false; }
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
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t account_id_length = 0; std::memcpy(&account_id_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < account_id_length) { return false; }
		account_id.assign((char*)*_buf_, account_id_length); (*_buf_) += account_id_length; nSize -= account_id_length;
		if(false == AccountType_Serializer::Load(account_type, _buf_, nSize)) { return false; }
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t user_name_length = 0; std::memcpy(&user_name_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < user_name_length) { return false; }
		user_name.assign((char*)*_buf_, user_name_length); (*_buf_) += user_name_length; nSize -= user_name_length;
		return true;
	}
}; //MsgCliSvr_Create_Req
struct MsgCliSvr_Create_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Create_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Create_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_Create_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Create_Ans {
	enum { MSG_ID = 22 }; 
	ErrorCode	error_code;
	MsgSvrCli_Create_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(error_code);
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
		if(false == ErrorCode_Serializer::Store(_buf_, error_code)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(error_code, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_Create_Ans
struct MsgSvrCli_Create_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Create_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Create_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Create_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_Counter_Ntf {
	enum { MSG_ID = 31 }; 
	CounterType	counter_id;
	uint64_t	update_date;
	int32_t	count;
	MsgSvrCli_Counter_Ntf()	{
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
}; //MsgSvrCli_Counter_Ntf
struct MsgSvrCli_Counter_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Counter_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Counter_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Counter_Ntf& obj) { return obj.Size(); }
};

}}

#endif // __MessageUser_H__
