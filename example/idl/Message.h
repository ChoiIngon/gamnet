#ifndef __Message_H__
#define __Message_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
#include <stdexcept>
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
struct UserData {
	std::string	user_id;
	uint32_t	user_seq;
	uint32_t	frame;
	UserData()	{
		user_seq = 0;
		frame = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += user_id.length();
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
		size_t user_id_size = user_id.length();
		std::memcpy(*_buf_, &user_id_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, user_id.c_str(), user_id.length()); (*_buf_) += user_id.length();
		std::memcpy(*_buf_, &user_seq, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &frame, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&frame, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //UserData
struct UserData_Serializer {
	static bool Store(char** _buf_, const UserData& obj) { return obj.Store(_buf_); }
	static bool Load(UserData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const UserData& obj) { return obj.Size(); }
};
struct MsgCliSvr_Login_Req {
	enum { MSG_ID = 00011 }; 
	std::string	user_id;
	MsgCliSvr_Login_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += user_id.length();
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
		return true;
	}
}; //MsgCliSvr_Login_Req
struct MsgCliSvr_Login_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Login_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Login_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_Login_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Login_Ans {
	enum { MSG_ID = 00012 }; 
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
struct MsgCliSvr_UpdateFrame_Req {
	enum { MSG_ID = 00021 }; 
	MsgCliSvr_UpdateFrame_Req()	{
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
}; //MsgCliSvr_UpdateFrame_Req
struct MsgCliSvr_UpdateFrame_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_UpdateFrame_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_UpdateFrame_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_UpdateFrame_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_UpdateFrame_Ans {
	enum { MSG_ID = 00022 }; 
	ErrorCode	error_code;
	uint32_t	frame;
	MsgSvrCli_UpdateFrame_Ans()	{
		frame = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(error_code);
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
		if(false == ErrorCode_Serializer::Store(_buf_, error_code)) { return false; }
		std::memcpy(*_buf_, &frame, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&frame, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgSvrCli_UpdateFrame_Ans
struct MsgSvrCli_UpdateFrame_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_UpdateFrame_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_UpdateFrame_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_UpdateFrame_Ans& obj) { return obj.Size(); }
};
struct MsgCliSvr_JoinChannel_Req {
	enum { MSG_ID = 00031 }; 
	MsgCliSvr_JoinChannel_Req()	{
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
}; //MsgCliSvr_JoinChannel_Req
struct MsgCliSvr_JoinChannel_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_JoinChannel_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_JoinChannel_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_JoinChannel_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_JoinChannel_Ans {
	enum { MSG_ID = 00032 }; 
	ErrorCode	error_code;
	uint32_t	channel_seq;
	std::list<UserData >	user_datas;
	MsgSvrCli_JoinChannel_Ans()	{
		channel_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(error_code);
		nSize += sizeof(uint32_t);
		nSize += sizeof(int32_t);
		for(std::list<UserData >::const_iterator user_datas_itr = user_datas.begin(); user_datas_itr != user_datas.end(); user_datas_itr++)	{
			const UserData& user_datas_elmt = *user_datas_itr;
			nSize += UserData_Serializer::Size(user_datas_elmt);
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
		if(false == ErrorCode_Serializer::Store(_buf_, error_code)) { return false; }
		std::memcpy(*_buf_, &channel_seq, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		size_t user_datas_size = user_datas.size();
		std::memcpy(*_buf_, &user_datas_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<UserData >::const_iterator user_datas_itr = user_datas.begin(); user_datas_itr != user_datas.end(); user_datas_itr++)	{
			const UserData& user_datas_elmt = *user_datas_itr;
			if(false == UserData_Serializer::Store(_buf_, user_datas_elmt)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(error_code, _buf_, nSize)) { return false; }
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&channel_seq, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t user_datas_length = 0; std::memcpy(&user_datas_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<user_datas_length; i++) {
			UserData user_datas_val;
			if(false == UserData_Serializer::Load(user_datas_val, _buf_, nSize)) { return false; }
			user_datas.push_back(user_datas_val);
		}
		return true;
	}
}; //MsgSvrCli_JoinChannel_Ans
struct MsgSvrCli_JoinChannel_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_JoinChannel_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_JoinChannel_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_JoinChannel_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_JoinChannel_Ntf {
	enum { MSG_ID = 00033 }; 
	uint32_t	user_count;
	UserData	new_user_data;
	MsgSvrCli_JoinChannel_Ntf()	{
		user_count = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t);
		nSize += UserData_Serializer::Size(new_user_data);
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
		std::memcpy(*_buf_, &user_count, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		if(false == UserData_Serializer::Store(_buf_, new_user_data)) { return false; }
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&user_count, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(false == UserData_Serializer::Load(new_user_data, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_JoinChannel_Ntf
struct MsgSvrCli_JoinChannel_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_JoinChannel_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_JoinChannel_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_JoinChannel_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_SendMessage_Req {
	enum { MSG_ID = 00041 }; 
	std::string	text;
	MsgCliSvr_SendMessage_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += text.length();
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
		size_t text_size = text.length();
		std::memcpy(*_buf_, &text_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, text.c_str(), text.length()); (*_buf_) += text.length();
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
		uint32_t text_length = 0; std::memcpy(&text_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < text_length) { return false; }
		text.assign((char*)*_buf_, text_length); (*_buf_) += text_length; nSize -= text_length;
		return true;
	}
}; //MsgCliSvr_SendMessage_Req
struct MsgCliSvr_SendMessage_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_SendMessage_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_SendMessage_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_SendMessage_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_SendMessage_Ans {
	enum { MSG_ID = 00042 }; 
	ErrorCode	error_code;
	MsgSvrCli_SendMessage_Ans()	{
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
}; //MsgSvrCli_SendMessage_Ans
struct MsgSvrCli_SendMessage_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_SendMessage_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_SendMessage_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_SendMessage_Ans& obj) { return obj.Size(); }
};
struct MsgSvrSvr_SendMessage_Req {
	enum { MSG_ID = 00043 }; 
	std::string	text;
	MsgSvrSvr_SendMessage_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += text.length();
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
		size_t text_size = text.length();
		std::memcpy(*_buf_, &text_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, text.c_str(), text.length()); (*_buf_) += text.length();
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
		uint32_t text_length = 0; std::memcpy(&text_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < text_length) { return false; }
		text.assign((char*)*_buf_, text_length); (*_buf_) += text_length; nSize -= text_length;
		return true;
	}
}; //MsgSvrSvr_SendMessage_Req
struct MsgSvrSvr_SendMessage_Req_Serializer {
	static bool Store(char** _buf_, const MsgSvrSvr_SendMessage_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrSvr_SendMessage_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrSvr_SendMessage_Req& obj) { return obj.Size(); }
};
struct MsgSvrSvr_SendMessage_Ans {
	enum { MSG_ID = 00044 }; 
	ErrorCode	error_code;
	MsgSvrSvr_SendMessage_Ans()	{
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
}; //MsgSvrSvr_SendMessage_Ans
struct MsgSvrSvr_SendMessage_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrSvr_SendMessage_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrSvr_SendMessage_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrSvr_SendMessage_Ans& obj) { return obj.Size(); }
};
struct MsgCliSvr_SendMessage_Ntf {
	enum { MSG_ID = 00045 }; 
	std::string	text;
	MsgCliSvr_SendMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += text.length();
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
		size_t text_size = text.length();
		std::memcpy(*_buf_, &text_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, text.c_str(), text.length()); (*_buf_) += text.length();
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
		uint32_t text_length = 0; std::memcpy(&text_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < text_length) { return false; }
		text.assign((char*)*_buf_, text_length); (*_buf_) += text_length; nSize -= text_length;
		return true;
	}
}; //MsgCliSvr_SendMessage_Ntf
struct MsgCliSvr_SendMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_SendMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_SendMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_SendMessage_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_SendMessage_Ntf {
	enum { MSG_ID = 00046 }; 
	std::string	text;
	MsgSvrCli_SendMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += text.length();
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
		size_t text_size = text.length();
		std::memcpy(*_buf_, &text_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, text.c_str(), text.length()); (*_buf_) += text.length();
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
		uint32_t text_length = 0; std::memcpy(&text_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < text_length) { return false; }
		text.assign((char*)*_buf_, text_length); (*_buf_) += text_length; nSize -= text_length;
		return true;
	}
}; //MsgSvrCli_SendMessage_Ntf
struct MsgSvrCli_SendMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_SendMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_SendMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_SendMessage_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrSvr_SendMessage_Ntf {
	enum { MSG_ID = 00047 }; 
	std::string	text;
	MsgSvrSvr_SendMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += text.length();
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
		size_t text_size = text.length();
		std::memcpy(*_buf_, &text_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, text.c_str(), text.length()); (*_buf_) += text.length();
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
		uint32_t text_length = 0; std::memcpy(&text_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < text_length) { return false; }
		text.assign((char*)*_buf_, text_length); (*_buf_) += text_length; nSize -= text_length;
		return true;
	}
}; //MsgSvrSvr_SendMessage_Ntf
struct MsgSvrSvr_SendMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrSvr_SendMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrSvr_SendMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrSvr_SendMessage_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_LeaveChannel_Req {
	enum { MSG_ID = 00051 }; 
	MsgCliSvr_LeaveChannel_Req()	{
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
}; //MsgCliSvr_LeaveChannel_Req
struct MsgCliSvr_LeaveChannel_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_LeaveChannel_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_LeaveChannel_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_LeaveChannel_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_LeaveChannel_Ans {
	enum { MSG_ID = 00052 }; 
	ErrorCode	error_code;
	MsgSvrCli_LeaveChannel_Ans()	{
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
}; //MsgSvrCli_LeaveChannel_Ans
struct MsgSvrCli_LeaveChannel_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_LeaveChannel_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_LeaveChannel_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_LeaveChannel_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_LeaveChannel_Ntf {
	enum { MSG_ID = 00053 }; 
	UserData	leave_user_data;
	MsgSvrCli_LeaveChannel_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += UserData_Serializer::Size(leave_user_data);
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
		if(false == UserData_Serializer::Store(_buf_, leave_user_data)) { return false; }
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
		if(false == UserData_Serializer::Load(leave_user_data, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_LeaveChannel_Ntf
struct MsgSvrCli_LeaveChannel_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_LeaveChannel_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_LeaveChannel_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_LeaveChannel_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_Kickout_Ntf {
	enum { MSG_ID = 00061 }; 
	ErrorCode	error_code;
	MsgSvrCli_Kickout_Ntf()	{
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
}; //MsgSvrCli_Kickout_Ntf
struct MsgSvrCli_Kickout_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Kickout_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Kickout_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Kickout_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_HeartBeat_Ntf {
	enum { MSG_ID = 00071 }; 
	int32_t	msg_seq;
	MsgCliSvr_HeartBeat_Ntf()	{
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
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
		std::memcpy(*_buf_, &msg_seq, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&msg_seq, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		return true;
	}
}; //MsgCliSvr_HeartBeat_Ntf
struct MsgCliSvr_HeartBeat_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_HeartBeat_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_HeartBeat_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_HeartBeat_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_HeartBeat_Ntf {
	enum { MSG_ID = 00072 }; 
	int32_t	msg_seq;
	MsgSvrCli_HeartBeat_Ntf()	{
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
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
		std::memcpy(*_buf_, &msg_seq, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&msg_seq, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		return true;
	}
}; //MsgSvrCli_HeartBeat_Ntf
struct MsgSvrCli_HeartBeat_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_HeartBeat_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_HeartBeat_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_HeartBeat_Ntf& obj) { return obj.Size(); }
};
#endif // __Message_H__
