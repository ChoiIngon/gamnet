#ifndef __MsgNetwork_H__
#define __MsgNetwork_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>

namespace Gamnet {

struct MsgNetwork_Connect_Req {
	enum { MSG_ID = 1 }; 
	std::string	user_id;
	MsgNetwork_Connect_Req()	{
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
}; //MsgNetwork_Connect_Req
struct MsgNetwork_Connect_Req_Serializer {
	static bool Store(char** _buf_, const MsgNetwork_Connect_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgNetwork_Connect_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgNetwork_Connect_Req& obj) { return obj.Size(); }
};
struct MsgNetwork_Connect_Ans {
	enum { MSG_ID = 1 }; 
	uint32_t	error_code;
	uint64_t	session_key;
	std::string	session_token;
	MsgNetwork_Connect_Ans()	{
		error_code = 0;
		session_key = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t);
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint32_t); nSize += session_token.length();
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
		std::memcpy(*_buf_, &error_code, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &session_key, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		size_t session_token_size = session_token.length();
		std::memcpy(*_buf_, &session_token_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, session_token.c_str(), session_token.length()); (*_buf_) += session_token.length();
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&error_code, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&session_key, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t session_token_length = 0; std::memcpy(&session_token_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < session_token_length) { return false; }
		session_token.assign((char*)*_buf_, session_token_length); (*_buf_) += session_token_length; nSize -= session_token_length;
		return true;
	}
}; //MsgNetwork_Connect_Ans
struct MsgNetwork_Connect_Ans_Serializer {
	static bool Store(char** _buf_, const MsgNetwork_Connect_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgNetwork_Connect_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgNetwork_Connect_Ans& obj) { return obj.Size(); }
};
struct MsgNetwork_Reconnect_Req {
	enum { MSG_ID = 2 }; 
	uint64_t	session_key;
	std::string	session_token;
	MsgNetwork_Reconnect_Req()	{
		session_key = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint32_t); nSize += session_token.length();
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
		std::memcpy(*_buf_, &session_key, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		size_t session_token_size = session_token.length();
		std::memcpy(*_buf_, &session_token_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, session_token.c_str(), session_token.length()); (*_buf_) += session_token.length();
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&session_key, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t session_token_length = 0; std::memcpy(&session_token_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < session_token_length) { return false; }
		session_token.assign((char*)*_buf_, session_token_length); (*_buf_) += session_token_length; nSize -= session_token_length;
		return true;
	}
}; //MsgNetwork_Reconnect_Req
struct MsgNetwork_Reconnect_Req_Serializer {
	static bool Store(char** _buf_, const MsgNetwork_Reconnect_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgNetwork_Reconnect_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgNetwork_Reconnect_Req& obj) { return obj.Size(); }
};
struct MsgNetwork_Reconnect_Ans {
	enum { MSG_ID = 2 }; 
	uint32_t	error_code;
	uint64_t	session_key;
	std::string	session_token;
	uint32_t	msg_seq;
	MsgNetwork_Reconnect_Ans()	{
		error_code = 0;
		session_key = 0;
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t);
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint32_t); nSize += session_token.length();
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
		std::memcpy(*_buf_, &error_code, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &session_key, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		size_t session_token_size = session_token.length();
		std::memcpy(*_buf_, &session_token_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, session_token.c_str(), session_token.length()); (*_buf_) += session_token.length();
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&error_code, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&session_key, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t session_token_length = 0; std::memcpy(&session_token_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < session_token_length) { return false; }
		session_token.assign((char*)*_buf_, session_token_length); (*_buf_) += session_token_length; nSize -= session_token_length;
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&msg_seq, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgNetwork_Reconnect_Ans
struct MsgNetwork_Reconnect_Ans_Serializer {
	static bool Store(char** _buf_, const MsgNetwork_Reconnect_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgNetwork_Reconnect_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgNetwork_Reconnect_Ans& obj) { return obj.Size(); }
};
struct MsgNetwork_Kickout_Ntf {
	enum { MSG_ID = 3 }; 
	uint32_t	error_code;
	MsgNetwork_Kickout_Ntf()	{
		error_code = 0;
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
		std::memcpy(*_buf_, &error_code, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&error_code, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgNetwork_Kickout_Ntf
struct MsgNetwork_Kickout_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgNetwork_Kickout_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgNetwork_Kickout_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgNetwork_Kickout_Ntf& obj) { return obj.Size(); }
};
struct MsgNetwork_HeartBeat_Req {
	enum { MSG_ID = 4 }; 
	MsgNetwork_HeartBeat_Req()	{
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
}; //MsgNetwork_HeartBeat_Req
struct MsgNetwork_HeartBeat_Req_Serializer {
	static bool Store(char** _buf_, const MsgNetwork_HeartBeat_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgNetwork_HeartBeat_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgNetwork_HeartBeat_Req& obj) { return obj.Size(); }
};
struct MsgNetwork_HeartBeat_Ans {
	enum { MSG_ID = 4 }; 
	MsgNetwork_HeartBeat_Ans()	{
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
}; //MsgNetwork_HeartBeat_Ans
struct MsgNetwork_HeartBeat_Ans_Serializer {
	static bool Store(char** _buf_, const MsgNetwork_HeartBeat_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgNetwork_HeartBeat_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgNetwork_HeartBeat_Ans& obj) { return obj.Size(); }
};

}

#endif // __MsgNetwork_H__
