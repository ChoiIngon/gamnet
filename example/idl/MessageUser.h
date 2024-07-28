#ifndef __MessageUser_H__
#define __MessageUser_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
#include <stdexcept>

#include "MessageCommon.h"
namespace Message { namespace User {

struct MsgCliSvr_Login_Req {
	enum { MSG_ID = 0x00001001 }; 
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
	enum { MSG_ID = 0x00001001 }; 
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
	enum { MSG_ID = 0x00001002 }; 
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
	enum { MSG_ID = 0x00001002 }; 
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
	enum { MSG_ID = 0x00001003 }; 
	std::list<CounterData >	counter_datas;
	MsgSvrCli_Counter_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int32_t);
		for(std::list<CounterData >::const_iterator counter_datas_itr = counter_datas.begin(); counter_datas_itr != counter_datas.end(); counter_datas_itr++)	{
			const CounterData& counter_datas_elmt = *counter_datas_itr;
			nSize += CounterData_Serializer::Size(counter_datas_elmt);
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
		size_t counter_datas_size = counter_datas.size();
		std::memcpy(*_buf_, &counter_datas_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<CounterData >::const_iterator counter_datas_itr = counter_datas.begin(); counter_datas_itr != counter_datas.end(); counter_datas_itr++)	{
			const CounterData& counter_datas_elmt = *counter_datas_itr;
			if(false == CounterData_Serializer::Store(_buf_, counter_datas_elmt)) { return false; }
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
		uint32_t counter_datas_length = 0; std::memcpy(&counter_datas_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<counter_datas_length; i++) {
			CounterData counter_datas_val;
			if(false == CounterData_Serializer::Load(counter_datas_val, _buf_, nSize)) { return false; }
			counter_datas.push_back(counter_datas_val);
		}
		return true;
	}
}; //MsgSvrCli_Counter_Ntf
struct MsgSvrCli_Counter_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Counter_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Counter_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Counter_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_Delete_Req {
	enum { MSG_ID = 0x00001004 }; 
	MsgCliSvr_Delete_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
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
}; //MsgCliSvr_Delete_Req
struct MsgCliSvr_Delete_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Delete_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Delete_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_Delete_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Delete_Ans {
	enum { MSG_ID = 0x00001004 }; 
	ErrorCode	error_code;
	MsgSvrCli_Delete_Ans()	{
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
}; //MsgSvrCli_Delete_Ans
struct MsgSvrCli_Delete_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Delete_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Delete_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Delete_Ans& obj) { return obj.Size(); }
};
struct MsgSvrSvr_Kickout_Ntf {
	enum { MSG_ID = 0x00001005 }; 
	MsgSvrSvr_Kickout_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
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
}; //MsgSvrSvr_Kickout_Ntf
struct MsgSvrSvr_Kickout_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrSvr_Kickout_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrSvr_Kickout_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrSvr_Kickout_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_Kickout_Ntf {
	enum { MSG_ID = 0x00001005 }; 
	MsgSvrCli_Kickout_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
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
}; //MsgSvrCli_Kickout_Ntf
struct MsgSvrCli_Kickout_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Kickout_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Kickout_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Kickout_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_Cheat_Req {
	enum { MSG_ID = 0x00001006 }; 
	std::string	command;
	std::list<std::string >	params;
	MsgCliSvr_Cheat_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += command.length();
		nSize += sizeof(int32_t);
		for(std::list<std::string >::const_iterator params_itr = params.begin(); params_itr != params.end(); params_itr++)	{
			const std::string& params_elmt = *params_itr;
			nSize += sizeof(uint32_t); nSize += params_elmt.length();
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
		size_t command_size = command.length();
		std::memcpy(*_buf_, &command_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, command.c_str(), command.length()); (*_buf_) += command.length();
		size_t params_size = params.size();
		std::memcpy(*_buf_, &params_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<std::string >::const_iterator params_itr = params.begin(); params_itr != params.end(); params_itr++)	{
			const std::string& params_elmt = *params_itr;
			size_t params_elmt_size = params_elmt.length();
			std::memcpy(*_buf_, &params_elmt_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			std::memcpy(*_buf_, params_elmt.c_str(), params_elmt.length()); (*_buf_) += params_elmt.length();
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
		uint32_t command_length = 0; std::memcpy(&command_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < command_length) { return false; }
		command.assign((char*)*_buf_, command_length); (*_buf_) += command_length; nSize -= command_length;
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t params_length = 0; std::memcpy(&params_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<params_length; i++) {
			std::string params_val;
			if(sizeof(int32_t) > nSize) { return false; }
			uint32_t params_val_length = 0; std::memcpy(&params_val_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
			if(nSize < params_val_length) { return false; }
			params_val.assign((char*)*_buf_, params_val_length); (*_buf_) += params_val_length; nSize -= params_val_length;
			params.push_back(params_val);
		}
		return true;
	}
}; //MsgCliSvr_Cheat_Req
struct MsgCliSvr_Cheat_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Cheat_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Cheat_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_Cheat_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Cheat_Ans {
	enum { MSG_ID = 0x00001006 }; 
	ErrorCode	error_code;
	MsgSvrCli_Cheat_Ans()	{
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
}; //MsgSvrCli_Cheat_Ans
struct MsgSvrCli_Cheat_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Cheat_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Cheat_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Cheat_Ans& obj) { return obj.Size(); }
};

}}

#endif // __MessageUser_H__
