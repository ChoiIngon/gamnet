#ifndef __Message_H__
#define __Message_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
struct Msg_CS_Echo_Req {
	enum { MSG_ID = 1 }; 
	std::string	Message;
	Msg_CS_Echo_Req()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
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
}; //Msg_CS_Echo_Req
struct Msg_CS_Echo_Req_Serializer {
	static bool Store(char** _buf_, const Msg_CS_Echo_Req& obj) { return obj.Store(_buf_); }
	static bool Load(Msg_CS_Echo_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const Msg_CS_Echo_Req& obj) { return obj.Size(); }
};
struct Msg_SC_Echo_Ans {
	enum { MSG_ID = 2 }; 
	std::string	Message;
	Msg_SC_Echo_Ans()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
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
}; //Msg_SC_Echo_Ans
struct Msg_SC_Echo_Ans_Serializer {
	static bool Store(char** _buf_, const Msg_SC_Echo_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(Msg_SC_Echo_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const Msg_SC_Echo_Ans& obj) { return obj.Size(); }
};
struct Msg_SS_Echo_Ntf {
	enum { MSG_ID = 3 }; 
	std::string	Message;
	Msg_SS_Echo_Ntf()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
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
}; //Msg_SS_Echo_Ntf
struct Msg_SS_Echo_Ntf_Serializer {
	static bool Store(char** _buf_, const Msg_SS_Echo_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(Msg_SS_Echo_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const Msg_SS_Echo_Ntf& obj) { return obj.Size(); }
};
struct Msg_SC_Echo_Ntf {
	enum { MSG_ID = 4 }; 
	std::string	Message;
	Msg_SC_Echo_Ntf()	{
	}
	int32_t Size() const {
		int32_t nSize = 0;
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
}; //Msg_SC_Echo_Ntf
struct Msg_SC_Echo_Ntf_Serializer {
	static bool Store(char** _buf_, const Msg_SC_Echo_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(Msg_SC_Echo_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static int32_t Size(const Msg_SC_Echo_Ntf& obj) { return obj.Size(); }
};
#endif // __Message_H__
