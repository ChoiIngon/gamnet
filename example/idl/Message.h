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
	uint32_t	Frame;
	GUserData()	{
		UserSEQ = 0;
		Frame = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += UserID.length();
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
		size_t UserID_size = UserID.length();
		std::memcpy(*_buf_, &UserID_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, UserID.c_str(), UserID.length()); (*_buf_) += UserID.length();
		std::memcpy(*_buf_, &UserSEQ, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &Frame, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&Frame, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //GUserData
struct GUserData_Serializer {
	static bool Store(char** _buf_, const GUserData& obj) { return obj.Store(_buf_); }
	static bool Load(GUserData& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const GUserData& obj) { return obj.Size(); }
};
struct MsgCliSvr_Login_Req {
	enum { MSG_ID = 1 }; 
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
	enum { MSG_ID = 1 }; 
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
struct MsgCliSvr_UpdateFrame_Req {
	enum { MSG_ID = 2 }; 
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
	enum { MSG_ID = 2 }; 
	GErrorCode	ErrorCode;
	uint32_t	Frame;
	MsgSvrCli_UpdateFrame_Ans()	{
		Frame = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += GErrorCode_Serializer::Size(ErrorCode);
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
		if(false == GErrorCode_Serializer::Store(_buf_, ErrorCode)) { return false; }
		std::memcpy(*_buf_, &Frame, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&Frame, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgSvrCli_UpdateFrame_Ans
struct MsgSvrCli_UpdateFrame_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_UpdateFrame_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_UpdateFrame_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_UpdateFrame_Ans& obj) { return obj.Size(); }
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
	uint32_t	ChannelSEQ;
	std::list<GUserData >	UserDatas;
	MsgSvrCli_JoinChannel_Ans()	{
		ChannelSEQ = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += GErrorCode_Serializer::Size(ErrorCode);
		nSize += sizeof(uint32_t);
		nSize += sizeof(int32_t);
		for(std::list<GUserData >::const_iterator UserDatas_itr = UserDatas.begin(); UserDatas_itr != UserDatas.end(); UserDatas_itr++)	{
			const GUserData& UserDatas_elmt = *UserDatas_itr;
			nSize += GUserData_Serializer::Size(UserDatas_elmt);
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
		if(false == GErrorCode_Serializer::Store(_buf_, ErrorCode)) { return false; }
		std::memcpy(*_buf_, &ChannelSEQ, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		size_t UserDatas_size = UserDatas.size();
		std::memcpy(*_buf_, &UserDatas_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<GUserData >::const_iterator UserDatas_itr = UserDatas.begin(); UserDatas_itr != UserDatas.end(); UserDatas_itr++)	{
			const GUserData& UserDatas_elmt = *UserDatas_itr;
			if(false == GUserData_Serializer::Store(_buf_, UserDatas_elmt)) { return false; }
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
		if(false == GErrorCode_Serializer::Load(ErrorCode, _buf_, nSize)) { return false; }
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&ChannelSEQ, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t UserDatas_length = 0; std::memcpy(&UserDatas_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<UserDatas_length; i++) {
			GUserData UserDatas_val;
			if(false == GUserData_Serializer::Load(UserDatas_val, _buf_, nSize)) { return false; }
			UserDatas.push_back(UserDatas_val);
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
	uint32_t	UserCount;
	GUserData	NewUserData;
	MsgSvrCli_JoinChannel_Ntf()	{
		UserCount = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t);
		nSize += GUserData_Serializer::Size(NewUserData);
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
		std::memcpy(*_buf_, &UserCount, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		if(false == GUserData_Serializer::Store(_buf_, NewUserData)) { return false; }
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&UserCount, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(false == GUserData_Serializer::Load(NewUserData, _buf_, nSize)) { return false; }
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
	std::string	UserID;
	std::string	Message;
	MsgSvrCli_SendMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += UserID.length();
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
		size_t UserID_size = UserID.length();
		std::memcpy(*_buf_, &UserID_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, UserID.c_str(), UserID.length()); (*_buf_) += UserID.length();
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
		uint32_t UserID_length = 0; std::memcpy(&UserID_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < UserID_length) { return false; }
		UserID.assign((char*)*_buf_, UserID_length); (*_buf_) += UserID_length; nSize -= UserID_length;
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
struct MsgSvrSvr_SendMessage_Ntf {
	enum { MSG_ID = 1022 }; 
	std::string	UserID;
	std::string	Message;
	MsgSvrSvr_SendMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += UserID.length();
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
		size_t UserID_size = UserID.length();
		std::memcpy(*_buf_, &UserID_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, UserID.c_str(), UserID.length()); (*_buf_) += UserID.length();
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
		uint32_t UserID_length = 0; std::memcpy(&UserID_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < UserID_length) { return false; }
		UserID.assign((char*)*_buf_, UserID_length); (*_buf_) += UserID_length; nSize -= UserID_length;
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
	GUserData	LeaveUserData;
	MsgSvrCli_LeaveChannel_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += GUserData_Serializer::Size(LeaveUserData);
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
		if(false == GUserData_Serializer::Store(_buf_, LeaveUserData)) { return false; }
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
		if(false == GUserData_Serializer::Load(LeaveUserData, _buf_, nSize)) { return false; }
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
	GErrorCode	ErrorCode;
	MsgSvrCli_Kickout_Ntf()	{
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
