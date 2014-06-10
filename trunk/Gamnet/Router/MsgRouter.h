#ifndef __MsgRouter_H__
#define __MsgRouter_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
enum ROUTER_ERROR_CODE {
		ROUTER_ERROR_SUCCESS,
		ROUTER_ERROR_MESSAGE_FORMAT_ERROR,
}; // ROUTER_ERROR_CODE
struct ROUTER_ERROR_CODE_Serializer {
	static bool Store(char** _buf_, const ROUTER_ERROR_CODE& obj) { 
		(*(ROUTER_ERROR_CODE*)(*_buf_)) = obj;	(*_buf_) += sizeof(ROUTER_ERROR_CODE);
		return true;
	}
	static bool Load(ROUTER_ERROR_CODE& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(ROUTER_ERROR_CODE) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(ROUTER_ERROR_CODE));		(*_buf_) += sizeof(ROUTER_ERROR_CODE); nSize -= sizeof(ROUTER_ERROR_CODE);
		return true;
	}
	static int32_t Size(const ROUTER_ERROR_CODE& obj) { return sizeof(ROUTER_ERROR_CODE); }
};
enum ROUTER_CAST_TYPE {
		ROUTER_CAST_UNI,
		ROUTER_CAST_MULTI,
		ROUTER_CAST_ANY,
		ROUTER_CAST_MAX,
}; // ROUTER_CAST_TYPE
struct ROUTER_CAST_TYPE_Serializer {
	static bool Store(char** _buf_, const ROUTER_CAST_TYPE& obj) { 
		(*(ROUTER_CAST_TYPE*)(*_buf_)) = obj;	(*_buf_) += sizeof(ROUTER_CAST_TYPE);
		return true;
	}
	static bool Load(ROUTER_CAST_TYPE& obj, const char** _buf_, size_t& nSize) { 
		if(sizeof(ROUTER_CAST_TYPE) > nSize) { return false; }		std::memcpy(&obj, *_buf_, sizeof(ROUTER_CAST_TYPE));		(*_buf_) += sizeof(ROUTER_CAST_TYPE); nSize -= sizeof(ROUTER_CAST_TYPE);
		return true;
	}
	static int32_t Size(const ROUTER_CAST_TYPE& obj) { return sizeof(ROUTER_CAST_TYPE); }
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
	int32_t Size() const {
		int32_t nSize = 0;
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
	static int32_t Size(const Address& obj) { return obj.Size(); }
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
			if(lhs.cast_type < rhs.cast_type)
			{
				return true;
			}
		}
	}
	return false;
}
inline bool operator == (const Address& lhs, const Address& rhs)
{
	if(lhs.service_name != rhs.service_name ||
	   lhs.cast_type != rhs.cast_type ||
	   lhs.id != rhs.id
	)
	{
		return false;
	}	
	return true;
}
inline bool operator != (const Address& lhs, const Address& rhs)
{
	if(lhs.service_name != rhs.service_name ||
	   lhs.cast_type != rhs.cast_type ||
	   lhs.id != rhs.id
	)
	{
		return true;
	}	
	return false;
}

struct MsgRouter_SetAddress_Req {
	enum { MSG_ID = 1 }; 
	Address	tLocalAddr;
	MsgRouter_SetAddress_Req()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += Address_Serializer::Size(tLocalAddr);
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
		if(false == Address_Serializer::Store(_buf_, tLocalAddr)) { return false; }
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
		if(false == Address_Serializer::Load(tLocalAddr, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgRouter_SetAddress_Req
struct MsgRouter_SetAddress_Req_Serializer {
	static bool Store(char** _buf_, const MsgRouter_SetAddress_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_SetAddress_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgRouter_SetAddress_Req& obj) { return obj.Size(); }
};
struct MsgRouter_SetAddress_Ans {
	enum { MSG_ID = 2 }; 
	uint16_t	nErrorCode;
	Address	tRemoteAddr;
	MsgRouter_SetAddress_Ans()	{
		nErrorCode = 0;
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += sizeof(uint16_t);
		nSize += Address_Serializer::Size(tRemoteAddr);
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
		std::memcpy(*_buf_, &nErrorCode, sizeof(uint16_t)); (*_buf_) += sizeof(uint16_t);
		if(false == Address_Serializer::Store(_buf_, tRemoteAddr)) { return false; }
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
		if(sizeof(uint16_t) > nSize) { return false; }	std::memcpy(&nErrorCode, *_buf_, sizeof(uint16_t));	(*_buf_) += sizeof(uint16_t); nSize -= sizeof(uint16_t);
		if(false == Address_Serializer::Load(tRemoteAddr, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgRouter_SetAddress_Ans
struct MsgRouter_SetAddress_Ans_Serializer {
	static bool Store(char** _buf_, const MsgRouter_SetAddress_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_SetAddress_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgRouter_SetAddress_Ans& obj) { return obj.Size(); }
};
struct MsgRouter_SetAddress_Ntf {
	enum { MSG_ID = 3 }; 
	MsgRouter_SetAddress_Ntf()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
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
	static int32_t Size(const MsgRouter_SetAddress_Ntf& obj) { return obj.Size(); }
};
struct MsgRouter_SendMsg_Ntf {
	enum { MSG_ID = 4 }; 
	uint64_t	nKey;
	std::string	sBuf;
	MsgRouter_SendMsg_Ntf()	{
		nKey = 0;
	}
	int32_t Size() const {
		int32_t nSize = 0;
		nSize += sizeof(uint64_t);
		nSize += sizeof(uint32_t); nSize += sBuf.length();
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
		std::memcpy(*_buf_, &nKey, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		size_t sBuf_size = sBuf.length();
		std::memcpy(*_buf_, &sBuf_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, sBuf.c_str(), sBuf.length()); (*_buf_) += sBuf.length();
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&nKey, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t sBuf_length = 0; std::memcpy(&sBuf_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < sBuf_length) { return false; }
		sBuf.assign((char*)*_buf_, sBuf_length); (*_buf_) += sBuf_length; nSize -= sBuf_length;
		return true;
	}
}; //MsgRouter_SendMsg_Ntf
struct MsgRouter_SendMsg_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgRouter_SendMsg_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgRouter_SendMsg_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const MsgRouter_SendMsg_Ntf& obj) { return obj.Size(); }
};
#endif // __MsgRouter_H__
