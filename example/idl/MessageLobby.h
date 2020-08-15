#ifndef __MessageLobby_H__
#define __MessageLobby_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>

#include "MessageCommon.h"
namespace Handler{ namespace Lobby {

struct MsgCliSvr_Join_Req {
	enum { MSG_ID = 120000001 }; 
	MsgCliSvr_Join_Req()	{
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
}; //MsgCliSvr_Join_Req
struct MsgCliSvr_Join_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_Join_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_Join_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_Join_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_Join_Ans {
	enum { MSG_ID = 120000002 }; 
	ErrorCode	error_code;
	MsgSvrCli_Join_Ans()	{
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
}; //MsgSvrCli_Join_Ans
struct MsgSvrCli_Join_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Join_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Join_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Join_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_Mail_Ntf {
	enum { MSG_ID = 120000003 }; 
	std::list<MailData >	mail_datas;
	MsgSvrCli_Mail_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int32_t);
		for(std::list<MailData >::const_iterator mail_datas_itr = mail_datas.begin(); mail_datas_itr != mail_datas.end(); mail_datas_itr++)	{
			const MailData& mail_datas_elmt = *mail_datas_itr;
			nSize += MailData_Serializer::Size(mail_datas_elmt);
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
		size_t mail_datas_size = mail_datas.size();
		std::memcpy(*_buf_, &mail_datas_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<MailData >::const_iterator mail_datas_itr = mail_datas.begin(); mail_datas_itr != mail_datas.end(); mail_datas_itr++)	{
			const MailData& mail_datas_elmt = *mail_datas_itr;
			if(false == MailData_Serializer::Store(_buf_, mail_datas_elmt)) { return false; }
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
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t mail_datas_length = 0; std::memcpy(&mail_datas_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<mail_datas_length; i++) {
			MailData mail_datas_val;
			if(false == MailData_Serializer::Load(mail_datas_val, _buf_, nSize)) { return false; }
			mail_datas.push_back(mail_datas_val);
		}
		return true;
	}
}; //MsgSvrCli_Mail_Ntf
struct MsgSvrCli_Mail_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_Mail_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_Mail_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_Mail_Ntf& obj) { return obj.Size(); }
};
struct MsgCliSvr_OpenMail_Req {
	enum { MSG_ID = 120000004 }; 
	uint64_t	mail_seq;
	MsgCliSvr_OpenMail_Req()	{
		mail_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
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
		std::memcpy(*_buf_, &mail_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&mail_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		return true;
	}
}; //MsgCliSvr_OpenMail_Req
struct MsgCliSvr_OpenMail_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_OpenMail_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_OpenMail_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_OpenMail_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_OpenMail_Ans {
	enum { MSG_ID = 120000005 }; 
	ErrorCode	error_code;
	MsgSvrCli_OpenMail_Ans()	{
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
}; //MsgSvrCli_OpenMail_Ans
struct MsgSvrCli_OpenMail_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_OpenMail_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_OpenMail_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_OpenMail_Ans& obj) { return obj.Size(); }
};

}}

#endif // __MessageLobby_H__
