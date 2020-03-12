#ifndef __Message_H__
#define __Message_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
enum class GErrorCode {
	Success = 0,
	MessageFormatError = 1000,
	MessageSeqOmmitError = 1001,
	InvalidUserError = 2000,
	InvalidAccessTokenError = 2001,
	DuplicateConnectionError = 3000,
	ReconnectTimeoutError = 4000,
	ResponseTimeoutError = 4001,
	AlreadyLoginSessionError = 5000,
	CanNotCreateCastGroup = 5001,
}; // GErrorCode
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<GErrorCode>(GErrorCode e) { 
	static const std::map<GErrorCode, std::string> table = {
		{ GErrorCode::Success, "Success"},
		{ GErrorCode::MessageFormatError, "MessageFormatError"},
		{ GErrorCode::MessageSeqOmmitError, "MessageSeqOmmitError"},
		{ GErrorCode::InvalidUserError, "InvalidUserError"},
		{ GErrorCode::InvalidAccessTokenError, "InvalidAccessTokenError"},
		{ GErrorCode::DuplicateConnectionError, "DuplicateConnectionError"},
		{ GErrorCode::ReconnectTimeoutError, "ReconnectTimeoutError"},
		{ GErrorCode::ResponseTimeoutError, "ResponseTimeoutError"},
		{ GErrorCode::AlreadyLoginSessionError, "AlreadyLoginSessionError"},
		{ GErrorCode::CanNotCreateCastGroup, "CanNotCreateCastGroup"},
	};
	auto itr = table.find(e); 
	if(table.end() == itr) { throw std::runtime_error("ToString<GErrorCode>()"); }
	return itr->second;
}
template<class T> T Parse(const std::string&);
template <> inline GErrorCode Parse<GErrorCode>(const std::string& s) {
	static const std::map<std::string, GErrorCode> table = {
		{ "Success", GErrorCode::Success},
		{ "MessageFormatError", GErrorCode::MessageFormatError},
		{ "MessageSeqOmmitError", GErrorCode::MessageSeqOmmitError},
		{ "InvalidUserError", GErrorCode::InvalidUserError},
		{ "InvalidAccessTokenError", GErrorCode::InvalidAccessTokenError},
		{ "DuplicateConnectionError", GErrorCode::DuplicateConnectionError},
		{ "ReconnectTimeoutError", GErrorCode::ReconnectTimeoutError},
		{ "ResponseTimeoutError", GErrorCode::ResponseTimeoutError},
		{ "AlreadyLoginSessionError", GErrorCode::AlreadyLoginSessionError},
		{ "CanNotCreateCastGroup", GErrorCode::CanNotCreateCastGroup},
	};
	auto itr = table.find(s); 
	if(table.end() == itr) { throw std::runtime_error("Parse<GErrorCode>()"); }
	return itr->second;
}
struct GErrorCode_Serializer {
	static bool Store(char** _buf_, const GErrorCode& obj) { 
		(*(GErrorCode*)(*_buf_)) = obj;	(*_buf_) += sizeof(GErrorCode);
		return true;
	}
	static bool Load(GErrorCode& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(GErrorCode) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(GErrorCode));		(*_buf_) += sizeof(GErrorCode); nSize -= sizeof(GErrorCode);
		return true;
	}
	static size_t Size(const GErrorCode& obj) { return sizeof(GErrorCode); }
};
struct GUserData {
	std::string	UserID;
	uint32_t	UserSEQ;
	GUserData()	{
		UserSEQ = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += UserID.length();
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
		size_t UserID_size = UserID.length();
		std::memcpy(*_buf_, &UserID_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, UserID.c_str(), UserID.length()); (*_buf_) += UserID.length();
		std::memcpy(*_buf_, &UserSEQ, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&UserSEQ, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //GUserData
struct GUserData_Serializer {
	static bool Store(char** _buf_, const GUserData& obj) { return obj.Store(_buf_); }
	static bool Load(GUserData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const GUserData& obj) { return obj.Size(); }
};
struct MsgCliSvr_Login_Req {
	enum { MSG_ID = 10000001 }; 
	std::string	UserID;
	MsgCliSvr_Login_Req()	{
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
}; //MsgCliSvr_Login_Req
struct MsgCliSvr_Login_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Login_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Login_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_Login_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Login_Ans {
	enum { MSG_ID = 10000001 }; 
	GErrorCode	ErrorCode;
	GUserData	UserData;
	MsgSvrCli_Login_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += GErrorCode_Serializer::Size(ErrorCode);
		nSize += GUserData_Serializer::Size(UserData);
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
		if(false == GErrorCode_Serializer::Store(_buf_, ErrorCode)) { return false; }
		if(false == GUserData_Serializer::Store(_buf_, UserData)) { return false; }
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
		if(false == GErrorCode_Serializer::Load(ErrorCode, _buf_, nSize)) { return false; }
		if(false == GUserData_Serializer::Load(UserData, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_Login_Ans
struct MsgSvrCli_Login_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Login_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Login_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Login_Ans& obj) { return obj.Size(); }
};
struct MsgCliSvr_JoinChannel_Req {
	enum { MSG_ID = 10001 }; 
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
	enum { MSG_ID = 10001 }; 
	GErrorCode	ErrorCode;
	MsgSvrCli_JoinChannel_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += GErrorCode_Serializer::Size(ErrorCode);
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
		if(false == GErrorCode_Serializer::Store(_buf_, ErrorCode)) { return false; }
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
		if(false == GErrorCode_Serializer::Load(ErrorCode, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_JoinChannel_Ans
struct MsgSvrCli_JoinChannel_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_JoinChannel_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_JoinChannel_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_JoinChannel_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_JoinChannel_Ntf {
	enum { MSG_ID = 10010001 }; 
	std::string	SessionKey;
	int32_t	SessionCount;
	MsgSvrCli_JoinChannel_Ntf()	{
		SessionCount = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += SessionKey.length();
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
		size_t SessionKey_size = SessionKey.length();
		std::memcpy(*_buf_, &SessionKey_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, SessionKey.c_str(), SessionKey.length()); (*_buf_) += SessionKey.length();
		std::memcpy(*_buf_, &SessionCount, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		uint32_t SessionKey_length = 0; std::memcpy(&SessionKey_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < SessionKey_length) { return false; }
		SessionKey.assign((char*)*_buf_, SessionKey_length); (*_buf_) += SessionKey_length; nSize -= SessionKey_length;
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&SessionCount, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		return true;
	}
}; //MsgSvrCli_JoinChannel_Ntf
struct MsgSvrCli_JoinChannel_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_JoinChannel_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_JoinChannel_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_JoinChannel_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_SendMessage_Ntf {
	enum { MSG_ID = 1020 }; 
	std::string	Message;
	MsgCliSvr_SendMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += Message.length();
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
		size_t Message_size = Message.length();
		std::memcpy(*_buf_, &Message_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, Message.c_str(), Message.length()); (*_buf_) += Message.length();
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
		uint32_t Message_length = 0; std::memcpy(&Message_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Message_length) { return false; }
		Message.assign((char*)*_buf_, Message_length); (*_buf_) += Message_length; nSize -= Message_length;
		return true;
	}
}; //MsgCliSvr_SendMessage_Ntf
struct MsgCliSvr_SendMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_SendMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_SendMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_SendMessage_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_SendMessage_Ntf {
	enum { MSG_ID = 1020 }; 
	std::string	Message;
	MsgSvrCli_SendMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += Message.length();
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
		size_t Message_size = Message.length();
		std::memcpy(*_buf_, &Message_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, Message.c_str(), Message.length()); (*_buf_) += Message.length();
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
		uint32_t Message_length = 0; std::memcpy(&Message_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Message_length) { return false; }
		Message.assign((char*)*_buf_, Message_length); (*_buf_) += Message_length; nSize -= Message_length;
		return true;
	}
}; //MsgSvrCli_SendMessage_Ntf
struct MsgSvrCli_SendMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_SendMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_SendMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_SendMessage_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrSvr_SendMessage_Req {
	enum { MSG_ID = 21 }; 
	std::string	Message;
	MsgSvrSvr_SendMessage_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += Message.length();
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
		size_t Message_size = Message.length();
		std::memcpy(*_buf_, &Message_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, Message.c_str(), Message.length()); (*_buf_) += Message.length();
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
		uint32_t Message_length = 0; std::memcpy(&Message_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Message_length) { return false; }
		Message.assign((char*)*_buf_, Message_length); (*_buf_) += Message_length; nSize -= Message_length;
		return true;
	}
}; //MsgSvrSvr_SendMessage_Req
struct MsgSvrSvr_SendMessage_Req_Serializer {
	static bool Store(char** _buf_, const MsgSvrSvr_SendMessage_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrSvr_SendMessage_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrSvr_SendMessage_Req& obj) { return obj.Size(); }
};
struct MsgSvrSvr_SendMessage_Ans {
	enum { MSG_ID = 22 }; 
	std::string	Message;
	MsgSvrSvr_SendMessage_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += Message.length();
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
		size_t Message_size = Message.length();
		std::memcpy(*_buf_, &Message_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, Message.c_str(), Message.length()); (*_buf_) += Message.length();
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
		uint32_t Message_length = 0; std::memcpy(&Message_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Message_length) { return false; }
		Message.assign((char*)*_buf_, Message_length); (*_buf_) += Message_length; nSize -= Message_length;
		return true;
	}
}; //MsgSvrSvr_SendMessage_Ans
struct MsgSvrSvr_SendMessage_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrSvr_SendMessage_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrSvr_SendMessage_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrSvr_SendMessage_Ans& obj) { return obj.Size(); }
};
struct MsgSvrSvr_SendMessage_Ntf {
	enum { MSG_ID = 1022 }; 
	std::string	Message;
	MsgSvrSvr_SendMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += Message.length();
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
		size_t Message_size = Message.length();
		std::memcpy(*_buf_, &Message_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, Message.c_str(), Message.length()); (*_buf_) += Message.length();
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
		uint32_t Message_length = 0; std::memcpy(&Message_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Message_length) { return false; }
		Message.assign((char*)*_buf_, Message_length); (*_buf_) += Message_length; nSize -= Message_length;
		return true;
	}
}; //MsgSvrSvr_SendMessage_Ntf
struct MsgSvrSvr_SendMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrSvr_SendMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrSvr_SendMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrSvr_SendMessage_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_LeaveChannel_Req {
	enum { MSG_ID = 10003 }; 
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
	enum { MSG_ID = 10003 }; 
	GErrorCode	ErrorCode;
	MsgSvrCli_LeaveChannel_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += GErrorCode_Serializer::Size(ErrorCode);
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
		if(false == GErrorCode_Serializer::Store(_buf_, ErrorCode)) { return false; }
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
		if(false == GErrorCode_Serializer::Load(ErrorCode, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_LeaveChannel_Ans
struct MsgSvrCli_LeaveChannel_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_LeaveChannel_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_LeaveChannel_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_LeaveChannel_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_LeaveChannel_Ntf {
	enum { MSG_ID = 10010003 }; 
	std::string	session_key;
	MsgSvrCli_LeaveChannel_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += session_key.length();
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
		size_t session_key_size = session_key.length();
		std::memcpy(*_buf_, &session_key_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, session_key.c_str(), session_key.length()); (*_buf_) += session_key.length();
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
		uint32_t session_key_length = 0; std::memcpy(&session_key_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < session_key_length) { return false; }
		session_key.assign((char*)*_buf_, session_key_length); (*_buf_) += session_key_length; nSize -= session_key_length;
		return true;
	}
}; //MsgSvrCli_LeaveChannel_Ntf
struct MsgSvrCli_LeaveChannel_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_LeaveChannel_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_LeaveChannel_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_LeaveChannel_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_Kickout_Ntf {
	enum { MSG_ID = 10000003 }; 
	GErrorCode	error_code;
	MsgSvrCli_Kickout_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += GErrorCode_Serializer::Size(error_code);
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
		if(false == GErrorCode_Serializer::Store(_buf_, error_code)) { return false; }
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
		if(false == GErrorCode_Serializer::Load(error_code, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_Kickout_Ntf
struct MsgSvrCli_Kickout_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Kickout_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Kickout_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Kickout_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_HeartBeat_Ntf {
	enum { MSG_ID = 10000004 }; 
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
	enum { MSG_ID = 10000004 }; 
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
