#ifndef __message_H__
#define __message_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>

#include "data.h"

struct MsgCliSvr_Login_Req {
	enum { MSG_ID = 1 }; 
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
	enum { MSG_ID = 1 }; 
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
	ErrorCode	error_code;
	MsgSvrCli_JoinChannel_Ans()	{
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
}; //MsgSvrCli_JoinChannel_Ans
struct MsgSvrCli_JoinChannel_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_JoinChannel_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_JoinChannel_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_JoinChannel_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_JoinChannel_Ntf {
	enum { MSG_ID = 10010001 }; 
	std::string	session_key;
	int32_t	session_count;
	MsgSvrCli_JoinChannel_Ntf()	{
		session_count = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += session_key.length();
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
		size_t session_key_size = session_key.length();
		std::memcpy(*_buf_, &session_key_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, session_key.c_str(), session_key.length()); (*_buf_) += session_key.length();
		std::memcpy(*_buf_, &session_count, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&session_count, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
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
	int64_t	msg_seq;
	MsgCliSvr_SendMessage_Ntf()	{
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int64_t);
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
		std::memcpy(*_buf_, &msg_seq, sizeof(int64_t)); (*_buf_) += sizeof(int64_t);
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
		if(sizeof(int64_t) > nSize) { return false; }	std::memcpy(&msg_seq, *_buf_, sizeof(int64_t));	(*_buf_) += sizeof(int64_t); nSize -= sizeof(int64_t);
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
	int64_t	msg_seq;
	MsgSvrCli_SendMessage_Ntf()	{
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int64_t);
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
		std::memcpy(*_buf_, &msg_seq, sizeof(int64_t)); (*_buf_) += sizeof(int64_t);
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
		if(sizeof(int64_t) > nSize) { return false; }	std::memcpy(&msg_seq, *_buf_, sizeof(int64_t));	(*_buf_) += sizeof(int64_t); nSize -= sizeof(int64_t);
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
	enum { MSG_ID = 10000004 }; 
	uint32_t	msg_seq;
	MsgCliSvr_HeartBeat_Ntf()	{
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
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
	static size_t Size(const MsgCliSvr_HeartBeat_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_HeartBeat_Ntf {
	enum { MSG_ID = 10000004 }; 
	uint32_t	msg_seq;
	MsgSvrCli_HeartBeat_Ntf()	{
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
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
}; //MsgSvrCli_HeartBeat_Ntf
struct MsgSvrCli_HeartBeat_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_HeartBeat_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_HeartBeat_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_HeartBeat_Ntf& obj) { return obj.Size(); }
};
#endif // __message_H__
