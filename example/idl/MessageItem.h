#ifndef __MessageItem_H__
#define __MessageItem_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
#include <stdexcept>

#include "MessageCommon.h"
namespace Message { namespace Item {

struct MsgCliSvr_OpenPackage_Req {
	enum { MSG_ID = 130000001 }; 
	uint64_t	item_seq;
	MsgCliSvr_OpenPackage_Req()	{
		item_seq = 0;
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
		std::memcpy(*_buf_, &item_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&item_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		return true;
	}
}; //MsgCliSvr_OpenPackage_Req
struct MsgCliSvr_OpenPackage_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_OpenPackage_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_OpenPackage_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_OpenPackage_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_OpenPackage_Ans {
	enum { MSG_ID = 130000002 }; 
	ErrorCode	error_code;
	ItemData	item_data;
	MsgSvrCli_OpenPackage_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(error_code);
		nSize += ItemData_Serializer::Size(item_data);
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
		if(false == ItemData_Serializer::Store(_buf_, item_data)) { return false; }
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
		if(false == ItemData_Serializer::Load(item_data, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_OpenPackage_Ans
struct MsgSvrCli_OpenPackage_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_OpenPackage_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_OpenPackage_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_OpenPackage_Ans& obj) { return obj.Size(); }
};
struct MsgSvrCli_AddItem_Ntf {
	enum { MSG_ID = 130000003 }; 
	std::list<ItemData >	item_datas;
	MsgSvrCli_AddItem_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int32_t);
		for(std::list<ItemData >::const_iterator item_datas_itr = item_datas.begin(); item_datas_itr != item_datas.end(); item_datas_itr++)	{
			const ItemData& item_datas_elmt = *item_datas_itr;
			nSize += ItemData_Serializer::Size(item_datas_elmt);
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
		size_t item_datas_size = item_datas.size();
		std::memcpy(*_buf_, &item_datas_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<ItemData >::const_iterator item_datas_itr = item_datas.begin(); item_datas_itr != item_datas.end(); item_datas_itr++)	{
			const ItemData& item_datas_elmt = *item_datas_itr;
			if(false == ItemData_Serializer::Store(_buf_, item_datas_elmt)) { return false; }
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
		uint32_t item_datas_length = 0; std::memcpy(&item_datas_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<item_datas_length; i++) {
			ItemData item_datas_val;
			if(false == ItemData_Serializer::Load(item_datas_val, _buf_, nSize)) { return false; }
			item_datas.push_back(item_datas_val);
		}
		return true;
	}
}; //MsgSvrCli_AddItem_Ntf
struct MsgSvrCli_AddItem_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_AddItem_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_AddItem_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_AddItem_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_UpdateItem_Ntf {
	enum { MSG_ID = 130000004 }; 
	std::list<ItemData >	item_datas;
	MsgSvrCli_UpdateItem_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(int32_t);
		for(std::list<ItemData >::const_iterator item_datas_itr = item_datas.begin(); item_datas_itr != item_datas.end(); item_datas_itr++)	{
			const ItemData& item_datas_elmt = *item_datas_itr;
			nSize += ItemData_Serializer::Size(item_datas_elmt);
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
		size_t item_datas_size = item_datas.size();
		std::memcpy(*_buf_, &item_datas_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<ItemData >::const_iterator item_datas_itr = item_datas.begin(); item_datas_itr != item_datas.end(); item_datas_itr++)	{
			const ItemData& item_datas_elmt = *item_datas_itr;
			if(false == ItemData_Serializer::Store(_buf_, item_datas_elmt)) { return false; }
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
		uint32_t item_datas_length = 0; std::memcpy(&item_datas_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<item_datas_length; i++) {
			ItemData item_datas_val;
			if(false == ItemData_Serializer::Load(item_datas_val, _buf_, nSize)) { return false; }
			item_datas.push_back(item_datas_val);
		}
		return true;
	}
}; //MsgSvrCli_UpdateItem_Ntf
struct MsgSvrCli_UpdateItem_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_UpdateItem_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_UpdateItem_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_UpdateItem_Ntf& obj) { return obj.Size(); }
};

}}

#endif // __MessageItem_H__
