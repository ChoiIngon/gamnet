#ifndef __MsgRouter_H__
#define __MsgRouter_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>

namespace Gamnet { namespace Network { namespace Router {

struct ROUTER_CAST_TYPE {
	enum TYPE {
			UNI_CAST,
			MULTI_CAST,
			ANY_CAST,
			MAX,
	};
	TYPE type;
	ROUTER_CAST_TYPE() : type(UNI_CAST) {}
	ROUTER_CAST_TYPE(int obj) : type((TYPE)obj) {} 
	ROUTER_CAST_TYPE(TYPE obj) : type(obj) {}
	operator TYPE() const { return type; }
	operator int() const { return (int)type; }
	ROUTER_CAST_TYPE& operator = (const TYPE& type) { this->type = type; return *this; }
	bool operator == (const ROUTER_CAST_TYPE& obj) { return type == obj.type; }
	bool operator == (ROUTER_CAST_TYPE::TYPE type) { return this->type == type; }
	bool operator == (int value) { return this->type == value; }
	bool operator != (const ROUTER_CAST_TYPE& obj) { return type != obj.type; }
	bool operator != (ROUTER_CAST_TYPE::TYPE type) { return this->type != type; }
	bool operator != (int value) { return this->type != value; }
	bool operator < (const ROUTER_CAST_TYPE& obj) { return type < obj.type; }
	bool operator < (ROUTER_CAST_TYPE::TYPE type) { return this->type < type; }
	bool operator < (int value) { return this->type < value; }
	bool operator > (const ROUTER_CAST_TYPE& obj) { return type > obj.type; }
	bool operator > (ROUTER_CAST_TYPE::TYPE type) { return this->type > type; }
	bool operator > (int value) { return this->type > value; }
	bool operator <= (const ROUTER_CAST_TYPE& obj) { return type <= obj.type; }
	bool operator <= (ROUTER_CAST_TYPE::TYPE type) { return this->type <= type; }
	bool operator <= (int value) { return this->type <= value; }
	bool operator >= (const ROUTER_CAST_TYPE& obj) { return type >= obj.type; }
	bool operator >= (ROUTER_CAST_TYPE::TYPE type) { return this->type >= type; }
	bool operator >= (int value) { return this->type >= value; }
}; // ROUTER_CAST_TYPE
struct ROUTER_CAST_TYPE_Serializer {
	static bool Store(char** _buf_, const ROUTER_CAST_TYPE& obj) { 
		(*(ROUTER_CAST_TYPE::TYPE*)(*_buf_)) = obj.type;	(*_buf_) += sizeof(ROUTER_CAST_TYPE::TYPE);
		return true;
	}
	static bool Load(ROUTER_CAST_TYPE& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(ROUTER_CAST_TYPE::TYPE) > nSize) { return false; }		std::memcpy(&obj.type, *_buf_, sizeof(ROUTER_CAST_TYPE::TYPE));		(*_buf_) += sizeof(ROUTER_CAST_TYPE::TYPE); nSize -= sizeof(ROUTER_CAST_TYPE::TYPE);
		return true;
	}
	static size_t Size(const ROUTER_CAST_TYPE& obj) { return sizeof(ROUTER_CAST_TYPE::TYPE); }
};
struct Address {
	std::string	service_name;
	ROUTER_CAST_TYPE	cast_type;
	uint32_t	id;
	uint64_t	msg_seq;
	Address()	{
		id = 0;
		msg_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += service_name.length();
		nSize += ROUTER_CAST_TYPE_Serializer::Size(cast_type);
		nSize += sizeof(uint32_t);
		nSize += sizeof(uint64_t);
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
		if(false == ROUTER_CAST_TYPE_Serializer::Store(_buf_, cast_type)) { return false; }
		std::memcpy(*_buf_, &id, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
		std::memcpy(*_buf_, &msg_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
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
		if(false == ROUTER_CAST_TYPE_Serializer::Load(cast_type, _buf_, nSize)) { return false; }
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&id, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&msg_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		return true;
	}
}; //Address
struct Address_Serializer {
	static bool Store(char** _buf_, const Address& obj) { return obj.Store(_buf_); }
	static bool Load(Address& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const Address& obj) { return obj.Size(); }
};

inline bool operator < (const Address& lhs, const Address& rhs)
{
	if(lhs.id < rhs.id)
	{
		return true;
	}
	else if(lhs.id == rhs.id)
	{	
		if(lhs.service_name < rhs.service_name)
		{
			return true;
		}
		else if(lhs.service_name == rhs.service_name)
		{
			if((int)lhs.cast_type < (int)rhs.cast_type)
			{
				return true;
			}
		}
	}
	return false;
}
inline bool operator == (const Address& lhs, const Address& rhs)
{
	if(lhs.service_name != rhs.service_name || (int)lhs.cast_type != (int)rhs.cast_type || lhs.id != rhs.id)
	{
		return false;
	}	
	return true;
}
inline bool operator != (const Address& lhs, const Address& rhs)
{
	if(lhs.service_name != rhs.service_name || (int)lhs.cast_type != (int)rhs.cast_type || lhs.id != rhs.id)
	{
		return true;
	}	
	return false;
}

struct MsgRouter_SetAddress_Req {
	enum { MSG_ID = 1 }; 
	Address	local_address;
	MsgRouter_SetAddress_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += Address_Serializer::Size(local_address);
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
		if(false == Address_Serializer::Store(_buf_, local_address)) { return false; }
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
		if(false == Address_Serializer::Load(local_address, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgRouter_SetAddress_Req
struct MsgRouter_SetAddress_Req_Serializer {
	static bool Store(char** _buf_, const MsgRouter_SetAddress_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_SetAddress_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_SetAddress_Req& obj) { return obj.Size(); }
};
struct MsgRouter_SetAddress_Ans {
	enum { MSG_ID = 2 }; 
	int32_t	error_code;
	Address	remote_address;
	MsgRouter_SetAddress_Ans()	{
		error_code = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int32_t);
		nSize += Address_Serializer::Size(remote_address);
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
		if(false == Address_Serializer::Store(_buf_, remote_address)) { return false; }
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
		if(false == Address_Serializer::Load(remote_address, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgRouter_SetAddress_Ans
struct MsgRouter_SetAddress_Ans_Serializer {
	static bool Store(char** _buf_, const MsgRouter_SetAddress_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_SetAddress_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_SetAddress_Ans& obj) { return obj.Size(); }
};
struct MsgRouter_SetAddress_Ntf {
	enum { MSG_ID = 3 }; 
	MsgRouter_SetAddress_Ntf()	{
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
}; //MsgRouter_SetAddress_Ntf
struct MsgRouter_SetAddress_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgRouter_SetAddress_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_SetAddress_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgRouter_SetAddress_Ntf& obj) { return obj.Size(); }
};
struct MsgRouter_SendMsg_Ntf {
	enum { MSG_ID = 4 }; 
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
	enum { MSG_ID = 5 }; 
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
