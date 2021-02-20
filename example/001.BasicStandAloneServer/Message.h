#ifndef __Message_H__
#define __Message_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
#include <stdexcept>
struct MsgCliSvr_HelloWorld_Req {
	enum { MSG_ID = 1 }; 
	std::string	greeting;
	MsgCliSvr_HelloWorld_Req()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += greeting.length();
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
		size_t greeting_size = greeting.length();
		std::memcpy(*_buf_, &greeting_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, greeting.c_str(), greeting.length()); (*_buf_) += greeting.length();
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
		uint32_t greeting_length = 0; std::memcpy(&greeting_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < greeting_length) { return false; }
		greeting.assign((char*)*_buf_, greeting_length); (*_buf_) += greeting_length; nSize -= greeting_length;
		return true;
	}
}; //MsgCliSvr_HelloWorld_Req
struct MsgCliSvr_HelloWorld_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_HelloWorld_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_HelloWorld_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_HelloWorld_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_HelloWorld_Ans {
	enum { MSG_ID = 1 }; 
	std::string	answer;
	MsgSvrCli_HelloWorld_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += answer.length();
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
		size_t answer_size = answer.length();
		std::memcpy(*_buf_, &answer_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, answer.c_str(), answer.length()); (*_buf_) += answer.length();
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
		uint32_t answer_length = 0; std::memcpy(&answer_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < answer_length) { return false; }
		answer.assign((char*)*_buf_, answer_length); (*_buf_) += answer_length; nSize -= answer_length;
		return true;
	}
}; //MsgSvrCli_HelloWorld_Ans
struct MsgSvrCli_HelloWorld_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_HelloWorld_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_HelloWorld_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_HelloWorld_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_Welcome_Ntf {
	enum { MSG_ID = 2 }; 
	std::string	greeting;
	MsgSvrCli_Welcome_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += greeting.length();
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
		size_t greeting_size = greeting.length();
		std::memcpy(*_buf_, &greeting_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, greeting.c_str(), greeting.length()); (*_buf_) += greeting.length();
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
		uint32_t greeting_length = 0; std::memcpy(&greeting_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < greeting_length) { return false; }
		greeting.assign((char*)*_buf_, greeting_length); (*_buf_) += greeting_length; nSize -= greeting_length;
		return true;
	}
}; //MsgSvrCli_Welcome_Ntf
struct MsgSvrCli_Welcome_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Welcome_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Welcome_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Welcome_Ntf& obj) { return obj.Size(); }
};
#endif // __Message_H__
