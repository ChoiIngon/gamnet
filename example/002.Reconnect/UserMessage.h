#ifndef __UserMessage_H__
#define __UserMessage_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
enum class ErrorCode {
	Success,
	MessageFormatError,
	DuplicateConnection,
}; // ErrorCode
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<ErrorCode>(ErrorCode e) { 
	static const std::map<ErrorCode, std::string> table = {
		{ ErrorCode::Success, "Success"},
		{ ErrorCode::MessageFormatError, "MessageFormatError"},
		{ ErrorCode::DuplicateConnection, "DuplicateConnection"},
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
		{ "DuplicateConnection", ErrorCode::DuplicateConnection},
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
	std::string	UserID;
	int64_t	UserSEQ;
	std::string	Data;
	UserData()	{
		UserSEQ = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += UserID.length();
		nSize += sizeof(int64_t);
		nSize += sizeof(uint32_t); nSize += Data.length();
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
		size_t UserID_size = UserID.length();
		std::memcpy(*_buf_, &UserID_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, UserID.c_str(), UserID.length()); (*_buf_) += UserID.length();
		std::memcpy(*_buf_, &UserSEQ, sizeof(int64_t)); (*_buf_) += sizeof(int64_t);
		size_t Data_size = Data.length();
		std::memcpy(*_buf_, &Data_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, Data.c_str(), Data.length()); (*_buf_) += Data.length();
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
		uint32_t UserID_length = 0; std::memcpy(&UserID_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < UserID_length) { return false; }
		UserID.assign((char*)*_buf_, UserID_length); (*_buf_) += UserID_length; nSize -= UserID_length;
		if(sizeof(int64_t) > nSize) { return false; }	std::memcpy(&UserSEQ, *_buf_, sizeof(int64_t));	(*_buf_) += sizeof(int64_t); nSize -= sizeof(int64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t Data_length = 0; std::memcpy(&Data_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Data_length) { return false; }
		Data.assign((char*)*_buf_, Data_length); (*_buf_) += Data_length; nSize -= Data_length;
		return true;
	}
}; //UserData
struct UserData_Serializer {
	static bool Store(char** _buf_, const UserData& obj) { return obj.Store(_buf_); }
	static bool Load(UserData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const UserData& obj) { return obj.Size(); }
};
struct MsgCliSvr_UserLogin_Req {
	enum { MSG_ID = 10 }; 
	std::string	UserID;
	MsgCliSvr_UserLogin_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += UserID.length();
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
		size_t UserID_size = UserID.length();
		std::memcpy(*_buf_, &UserID_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, UserID.c_str(), UserID.length()); (*_buf_) += UserID.length();
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
		uint32_t UserID_length = 0; std::memcpy(&UserID_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < UserID_length) { return false; }
		UserID.assign((char*)*_buf_, UserID_length); (*_buf_) += UserID_length; nSize -= UserID_length;
		return true;
	}
}; //MsgCliSvr_UserLogin_Req
struct MsgCliSvr_UserLogin_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_UserLogin_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_UserLogin_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_UserLogin_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_UserLogin_Ans {
	enum { MSG_ID = 10 }; 
	ErrorCode	Error;
	UserData	User;
	MsgSvrCli_UserLogin_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(Error);
		nSize += UserData_Serializer::Size(User);
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
		if(false == ErrorCode_Serializer::Store(_buf_, Error)) { return false; }
		if(false == UserData_Serializer::Store(_buf_, User)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(Error, _buf_, nSize)) { return false; }
		if(false == UserData_Serializer::Load(User, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_UserLogin_Ans
struct MsgSvrCli_UserLogin_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_UserLogin_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_UserLogin_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_UserLogin_Ans& obj) { return obj.Size(); }
};
struct MsgCliSvr_UserUpdate_Req {
	enum { MSG_ID = 11 }; 
	std::string	Data;
	MsgCliSvr_UserUpdate_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += Data.length();
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
		size_t Data_size = Data.length();
		std::memcpy(*_buf_, &Data_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, Data.c_str(), Data.length()); (*_buf_) += Data.length();
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
		uint32_t Data_length = 0; std::memcpy(&Data_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Data_length) { return false; }
		Data.assign((char*)*_buf_, Data_length); (*_buf_) += Data_length; nSize -= Data_length;
		return true;
	}
}; //MsgCliSvr_UserUpdate_Req
struct MsgCliSvr_UserUpdate_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_UserUpdate_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_UserUpdate_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_UserUpdate_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_UserUpdate_Ans {
	enum { MSG_ID = 11 }; 
	ErrorCode	Error;
	UserData	User;
	MsgSvrCli_UserUpdate_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(Error);
		nSize += UserData_Serializer::Size(User);
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
		if(false == ErrorCode_Serializer::Store(_buf_, Error)) { return false; }
		if(false == UserData_Serializer::Store(_buf_, User)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(Error, _buf_, nSize)) { return false; }
		if(false == UserData_Serializer::Load(User, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_UserUpdate_Ans
struct MsgSvrCli_UserUpdate_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_UserUpdate_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_UserUpdate_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_UserUpdate_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_UserKickout_Ntf {
	enum { MSG_ID = 10000012 }; 
	ErrorCode	Error;
	MsgSvrCli_UserKickout_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(Error);
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
		if(false == ErrorCode_Serializer::Store(_buf_, Error)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(Error, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_UserKickout_Ntf
struct MsgSvrCli_UserKickout_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_UserKickout_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_UserKickout_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_UserKickout_Ntf& obj) { return obj.Size(); }
};
#endif // __UserMessage_H__
