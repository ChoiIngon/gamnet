#ifndef __EchoMessage_H__
#define __EchoMessage_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
enum class ErrorCode {
	Success,
	MessageFormatError,
}; // ErrorCode
template <class T> const std::string& ToString(T);
template <> inline const std::string& ToString<ErrorCode>(ErrorCode e) { 
	static const std::map<ErrorCode, std::string> table = {
		{ ErrorCode::Success, "Success"},
		{ ErrorCode::MessageFormatError, "MessageFormatError"},
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
struct MsgCliSvr_EchoMessage_Req {
	enum { MSG_ID = 10 }; 
	std::string	Message;
	MsgCliSvr_EchoMessage_Req()	{
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
}; //MsgCliSvr_EchoMessage_Req
struct MsgCliSvr_EchoMessage_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_EchoMessage_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_EchoMessage_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_EchoMessage_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_EchoMessage_Ans {
	enum { MSG_ID = 10 }; 
	ErrorCode	Error;
	std::string	Message;
	MsgSvrCli_EchoMessage_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(Error);
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
		if(false == ErrorCode_Serializer::Store(_buf_, Error)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(Error, _buf_, nSize)) { return false; }
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t Message_length = 0; std::memcpy(&Message_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Message_length) { return false; }
		Message.assign((char*)*_buf_, Message_length); (*_buf_) += Message_length; nSize -= Message_length;
		return true;
	}
}; //MsgSvrCli_EchoMessage_Ans
struct MsgSvrCli_EchoMessage_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_EchoMessage_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_EchoMessage_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_EchoMessage_Ans& obj) { return obj.Size(); }
};
struct MsgSvrSvr_EchoMessage_Ntf {
	enum { MSG_ID = 10000010 }; 
	std::string	Message;
	MsgSvrSvr_EchoMessage_Ntf()	{
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
}; //MsgSvrSvr_EchoMessage_Ntf
struct MsgSvrSvr_EchoMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrSvr_EchoMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrSvr_EchoMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrSvr_EchoMessage_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_EchoMessage_Ntf {
	enum { MSG_ID = 10000010 }; 
	std::string	Messgae;
	MsgSvrCli_EchoMessage_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint32_t); nSize += Messgae.length();
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
		size_t Messgae_size = Messgae.length();
		std::memcpy(*_buf_, &Messgae_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, Messgae.c_str(), Messgae.length()); (*_buf_) += Messgae.length();
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
		uint32_t Messgae_length = 0; std::memcpy(&Messgae_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		if(nSize < Messgae_length) { return false; }
		Messgae.assign((char*)*_buf_, Messgae_length); (*_buf_) += Messgae_length; nSize -= Messgae_length;
		return true;
	}
}; //MsgSvrCli_EchoMessage_Ntf
struct MsgSvrCli_EchoMessage_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_EchoMessage_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_EchoMessage_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_EchoMessage_Ntf& obj) { return obj.Size(); }
};
#endif // __EchoMessage_H__
