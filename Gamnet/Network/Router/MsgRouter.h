#ifndef __MsgRouter_H__
#define __MsgRouter_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
#include <stdexcept>

#include <sstream>
#include <memory>
namespace Gamnet { namespace Network { namespace Router {

struct MsgRouter_Connect_Req {
	enum { MSG_ID = 001 }; 
	std::string	service_name;
	uint32_t	id;
	MsgRouter_Connect_Req()	{
		id = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += service_name.length();
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
		size_t service_name_size = service_name.length();
		std::memcpy(*_buf_, &service_name_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, service_name.c_str(), service_name.length()); (*_buf_) += service_name.length();
		std::memcpy(*_buf_, &id, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		uint32_t service_name_length = 0; std::memcpy(&service_name_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < service_name_length) { return false; }
		service_name.assign((char*)*_buf_, service_name_length); (*_buf_) += service_name_length; nSize -= service_name_length;
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&id, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgRouter_Connect_Req
struct MsgRouter_Connect_Req_Serializer {
	static bool Store(char** _buf_, const MsgRouter_Connect_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_Connect_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_Connect_Req& obj) { return obj.Size(); }
};
struct MsgRouter_Connect_Ans {
	enum { MSG_ID = 002 }; 
	int32_t	error_code;
	std::string	service_name;
	uint32_t	id;
	MsgRouter_Connect_Ans()	{
		error_code = 0;
		id = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int32_t);
		nSize += sizeof(uint32_t); nSize += service_name.length();
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
		std::memcpy(*_buf_, &error_code, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		size_t service_name_size = service_name.length();
		std::memcpy(*_buf_, &service_name_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, service_name.c_str(), service_name.length()); (*_buf_) += service_name.length();
		std::memcpy(*_buf_, &id, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&error_code, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t service_name_length = 0; std::memcpy(&service_name_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < service_name_length) { return false; }
		service_name.assign((char*)*_buf_, service_name_length); (*_buf_) += service_name_length; nSize -= service_name_length;
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&id, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgRouter_Connect_Ans
struct MsgRouter_Connect_Ans_Serializer {
	static bool Store(char** _buf_, const MsgRouter_Connect_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_Connect_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_Connect_Ans& obj) { return obj.Size(); }
};
struct MsgRouter_RegisterAddress_Req {
	enum { MSG_ID = 003 }; 
	std::string	service_name;
	uint32_t	id;
	int32_t	router_port;
	MsgRouter_RegisterAddress_Req()	{
		id = 0;
		router_port = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += service_name.length();
		nSize += sizeof(uint32_t);
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
		size_t service_name_size = service_name.length();
		std::memcpy(*_buf_, &service_name_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, service_name.c_str(), service_name.length()); (*_buf_) += service_name.length();
		std::memcpy(*_buf_, &id, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &router_port, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		uint32_t service_name_length = 0; std::memcpy(&service_name_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < service_name_length) { return false; }
		service_name.assign((char*)*_buf_, service_name_length); (*_buf_) += service_name_length; nSize -= service_name_length;
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&id, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&router_port, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		return true;
	}
}; //MsgRouter_RegisterAddress_Req
struct MsgRouter_RegisterAddress_Req_Serializer {
	static bool Store(char** _buf_, const MsgRouter_RegisterAddress_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_RegisterAddress_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_RegisterAddress_Req& obj) { return obj.Size(); }
};
struct MsgRouter_RegisterAddress_Ans {
	enum { MSG_ID = 004 }; 
	int32_t	error_code;
	std::string	service_name;
	uint32_t	id;
	int32_t	router_port;
	MsgRouter_RegisterAddress_Ans()	{
		error_code = 0;
		id = 0;
		router_port = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int32_t);
		nSize += sizeof(uint32_t); nSize += service_name.length();
		nSize += sizeof(uint32_t);
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
		std::memcpy(*_buf_, &error_code, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		size_t service_name_size = service_name.length();
		std::memcpy(*_buf_, &service_name_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, service_name.c_str(), service_name.length()); (*_buf_) += service_name.length();
		std::memcpy(*_buf_, &id, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &router_port, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
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
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&error_code, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t service_name_length = 0; std::memcpy(&service_name_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < service_name_length) { return false; }
		service_name.assign((char*)*_buf_, service_name_length); (*_buf_) += service_name_length; nSize -= service_name_length;
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&id, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&router_port, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		return true;
	}
}; //MsgRouter_RegisterAddress_Ans
struct MsgRouter_RegisterAddress_Ans_Serializer {
	static bool Store(char** _buf_, const MsgRouter_RegisterAddress_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_RegisterAddress_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_RegisterAddress_Ans& obj) { return obj.Size(); }
};
struct MsgRouter_RegisterAddress_Ntf {
	enum { MSG_ID = 005 }; 
	std::string	service_name;
	uint32_t	id;
	MsgRouter_RegisterAddress_Ntf()	{
		id = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += service_name.length();
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
		size_t service_name_size = service_name.length();
		std::memcpy(*_buf_, &service_name_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, service_name.c_str(), service_name.length()); (*_buf_) += service_name.length();
		std::memcpy(*_buf_, &id, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		uint32_t service_name_length = 0; std::memcpy(&service_name_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < service_name_length) { return false; }
		service_name.assign((char*)*_buf_, service_name_length); (*_buf_) += service_name_length; nSize -= service_name_length;
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&id, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgRouter_RegisterAddress_Ntf
struct MsgRouter_RegisterAddress_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgRouter_RegisterAddress_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_RegisterAddress_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_RegisterAddress_Ntf& obj) { return obj.Size(); }
};
struct MsgRouter_SendMsg_Ntf {
	enum { MSG_ID = 006 }; 
	uint32_t	msg_seq;
	std::string	buffer;
	MsgRouter_SendMsg_Ntf()	{
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t);
		nSize += sizeof(uint32_t); nSize += buffer.length();
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
		size_t buffer_size = buffer.length();
		std::memcpy(*_buf_, &buffer_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, buffer.c_str(), buffer.length()); (*_buf_) += buffer.length();
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
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t buffer_length = 0; std::memcpy(&buffer_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < buffer_length) { return false; }
		buffer.assign((char*)*_buf_, buffer_length); (*_buf_) += buffer_length; nSize -= buffer_length;
		return true;
	}
}; //MsgRouter_SendMsg_Ntf
struct MsgRouter_SendMsg_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgRouter_SendMsg_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_SendMsg_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_SendMsg_Ntf& obj) { return obj.Size(); }
};
struct MsgRouter_HeartBeat_Ntf {
	enum { MSG_ID = 007 }; 
	MsgRouter_HeartBeat_Ntf()	{
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
}; //MsgRouter_HeartBeat_Ntf
struct MsgRouter_HeartBeat_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgRouter_HeartBeat_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_HeartBeat_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_HeartBeat_Ntf& obj) { return obj.Size(); }
};

}}}

#endif // __MsgRouter_H__
