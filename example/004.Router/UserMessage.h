#ifndef __UserMessage_H__
#define __UserMessage_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
enum class ErrorCode {
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
}; // ErrorCode
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<ErrorCode>(ErrorCode e) { 
	static const std::map<ErrorCode, std::string> table = {
		{ ErrorCode::Success, "Success"},
		{ ErrorCode::MessageFormatError, "MessageFormatError"},
		{ ErrorCode::MessageSeqOmmitError, "MessageSeqOmmitError"},
		{ ErrorCode::InvalidUserError, "InvalidUserError"},
		{ ErrorCode::InvalidAccessTokenError, "InvalidAccessTokenError"},
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
struct ChatUserData {
	std::string	UserID;
	std::string	PersonalData;
	ChatUserData()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += UserID.length();
		nSize += sizeof(uint32_t); nSize += PersonalData.length();
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
		size_t PersonalData_size = PersonalData.length();
		std::memcpy(*_buf_, &PersonalData_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, PersonalData.c_str(), PersonalData.length()); (*_buf_) += PersonalData.length();
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
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t PersonalData_length = 0; std::memcpy(&PersonalData_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < PersonalData_length) { return false; }
		PersonalData.assign((char*)*_buf_, PersonalData_length); (*_buf_) += PersonalData_length; nSize -= PersonalData_length;
		return true;
	}
}; //ChatUserData
struct ChatUserData_Serializer {
	static bool Store(char** _buf_, const ChatUserData& obj) { return obj.Store(_buf_); }
	static bool Load(ChatUserData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const ChatUserData& obj) { return obj.Size(); }
};
struct MsgCliSvr_Login_Req {
	enum { MSG_ID = 10 }; 
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
	enum { MSG_ID = 10 }; 
	ErrorCode	Error;
	ChatUserData	UserData;
	MsgSvrCli_Login_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(Error);
		nSize += ChatUserData_Serializer::Size(UserData);
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
		if(false == ChatUserData_Serializer::Store(_buf_, UserData)) { return false; }
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
		if(false == ChatUserData_Serializer::Load(UserData, _buf_, nSize)) { return false; }
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
	ErrorCode	Error;
	int64_t	ChannelSEQ;
	std::list<ChatUserData >	ExistingUserData;
	MsgSvrCli_JoinChannel_Ans()	{
		ChannelSEQ = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(Error);
		nSize += sizeof(int64_t);
		nSize += sizeof(int32_t);
		for(std::list<ChatUserData >::const_iterator ExistingUserData_itr = ExistingUserData.begin(); ExistingUserData_itr != ExistingUserData.end(); ExistingUserData_itr++)	{
			const ChatUserData& ExistingUserData_elmt = *ExistingUserData_itr;
			nSize += ChatUserData_Serializer::Size(ExistingUserData_elmt);
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
		if(false == ErrorCode_Serializer::Store(_buf_, Error)) { return false; }
		std::memcpy(*_buf_, &ChannelSEQ, sizeof(int64_t)); (*_buf_) += sizeof(int64_t);
		size_t ExistingUserData_size = ExistingUserData.size();
		std::memcpy(*_buf_, &ExistingUserData_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<ChatUserData >::const_iterator ExistingUserData_itr = ExistingUserData.begin(); ExistingUserData_itr != ExistingUserData.end(); ExistingUserData_itr++)	{
			const ChatUserData& ExistingUserData_elmt = *ExistingUserData_itr;
			if(false == ChatUserData_Serializer::Store(_buf_, ExistingUserData_elmt)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(Error, _buf_, nSize)) { return false; }
		if(sizeof(int64_t) > nSize) { return false; }	std::memcpy(&ChannelSEQ, *_buf_, sizeof(int64_t));	(*_buf_) += sizeof(int64_t); nSize -= sizeof(int64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t ExistingUserData_length = 0; std::memcpy(&ExistingUserData_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<ExistingUserData_length; i++) {
			ChatUserData ExistingUserData_val;
			if(false == ChatUserData_Serializer::Load(ExistingUserData_val, _buf_, nSize)) { return false; }
			ExistingUserData.push_back(ExistingUserData_val);
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
	enum { MSG_ID = 10010001 }; 
	ChatUserData	NewUserData;
	MsgSvrCli_JoinChannel_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ChatUserData_Serializer::Size(NewUserData);
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
		if(false == ChatUserData_Serializer::Store(_buf_, NewUserData)) { return false; }
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
		if(false == ChatUserData_Serializer::Load(NewUserData, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_JoinChannel_Ntf
struct MsgSvrCli_JoinChannel_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_JoinChannel_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_JoinChannel_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_JoinChannel_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_SendMessage_Ntf {
	enum { MSG_ID = 10002 }; 
	int64_t	ChatSEQ;
	std::string	ChatMessage;
	MsgCliSvr_SendMessage_Ntf()	{
		ChatSEQ = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int64_t);
		nSize += sizeof(uint32_t); nSize += ChatMessage.length();
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
		std::memcpy(*_buf_, &ChatSEQ, sizeof(int64_t)); (*_buf_) += sizeof(int64_t);
		size_t ChatMessage_size = ChatMessage.length();
		std::memcpy(*_buf_, &ChatMessage_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, ChatMessage.c_str(), ChatMessage.length()); (*_buf_) += ChatMessage.length();
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
		if(sizeof(int64_t) > nSize) { return false; }	std::memcpy(&ChatSEQ, *_buf_, sizeof(int64_t));	(*_buf_) += sizeof(int64_t); nSize -= sizeof(int64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t ChatMessage_length = 0; std::memcpy(&ChatMessage_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < ChatMessage_length) { return false; }
		ChatMessage.assign((char*)*_buf_, ChatMessage_length); (*_buf_) += ChatMessage_length; nSize -= ChatMessage_length;
		return true;
	}
}; //MsgCliSvr_SendMessage_Ntf
struct MsgCliSvr_SendMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_SendMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_SendMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_SendMessage_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_SendMessage_Ntf {
	enum { MSG_ID = 10010002 }; 
	int64_t	ChatSEQ;
	std::string	ChatMessage;
	MsgSvrCli_SendMessage_Ntf()	{
		ChatSEQ = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int64_t);
		nSize += sizeof(uint32_t); nSize += ChatMessage.length();
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
		std::memcpy(*_buf_, &ChatSEQ, sizeof(int64_t)); (*_buf_) += sizeof(int64_t);
		size_t ChatMessage_size = ChatMessage.length();
		std::memcpy(*_buf_, &ChatMessage_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, ChatMessage.c_str(), ChatMessage.length()); (*_buf_) += ChatMessage.length();
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
		if(sizeof(int64_t) > nSize) { return false; }	std::memcpy(&ChatSEQ, *_buf_, sizeof(int64_t));	(*_buf_) += sizeof(int64_t); nSize -= sizeof(int64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t ChatMessage_length = 0; std::memcpy(&ChatMessage_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < ChatMessage_length) { return false; }
		ChatMessage.assign((char*)*_buf_, ChatMessage_length); (*_buf_) += ChatMessage_length; nSize -= ChatMessage_length;
		return true;
	}
}; //MsgSvrCli_SendMessage_Ntf
struct MsgSvrCli_SendMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_SendMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_SendMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_SendMessage_Ntf& obj) { return obj.Size(); }
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
	ErrorCode	Error;
	MsgSvrCli_LeaveChannel_Ans()	{
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
}; //MsgSvrCli_LeaveChannel_Ans
struct MsgSvrCli_LeaveChannel_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_LeaveChannel_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_LeaveChannel_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_LeaveChannel_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_LeaveChannel_Ntf {
	enum { MSG_ID = 10010003 }; 
	ChatUserData	LeaveUserData;
	MsgSvrCli_LeaveChannel_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ChatUserData_Serializer::Size(LeaveUserData);
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
		if(false == ChatUserData_Serializer::Store(_buf_, LeaveUserData)) { return false; }
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
		if(false == ChatUserData_Serializer::Load(LeaveUserData, _buf_, nSize)) { return false; }
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
	ErrorCode	Error;
	MsgSvrCli_Kickout_Ntf()	{
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
}; //MsgSvrCli_Kickout_Ntf
struct MsgSvrCli_Kickout_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Kickout_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Kickout_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Kickout_Ntf& obj) { return obj.Size(); }
};
#endif // __UserMessage_H__
