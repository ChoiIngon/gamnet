#ifndef __MessageDungeon_H__
#define __MessageDungeon_H__
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <stdint.h>
#include <stdexcept>

#include "MessageCommon.h"
namespace Message{ namespace Dungeon {

struct MsgCliSvr_CreateDungeon_Req {
	enum { MSG_ID = 40000001 }; 
	MsgCliSvr_CreateDungeon_Req()	{
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
}; //MsgCliSvr_CreateDungeon_Req
struct MsgCliSvr_CreateDungeon_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_CreateDungeon_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_CreateDungeon_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_CreateDungeon_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_CreateDungeon_Ans {
	enum { MSG_ID = 40000001 }; 
	ErrorCode	error_code;
	int32_t	width;
	int32_t	height;
	uint64_t	player_unit_seq;
	Vector2Int	start;
	std::vector<DungeonTileType >	tiles;
	std::vector<Monster >	monsters;
	MsgSvrCli_CreateDungeon_Ans()	{
		width = 0;
		height = 0;
		player_unit_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(error_code);
		nSize += sizeof(int32_t);
		nSize += sizeof(int32_t);
		nSize += sizeof(uint64_t);
		nSize += Vector2Int_Serializer::Size(start);
		nSize += sizeof(int32_t);
		for(std::vector<DungeonTileType >::const_iterator tiles_itr = tiles.begin(); tiles_itr != tiles.end(); tiles_itr++)	{
			const DungeonTileType& tiles_elmt = *tiles_itr;
			nSize += DungeonTileType_Serializer::Size(tiles_elmt);
		}
		nSize += sizeof(int32_t);
		for(std::vector<Monster >::const_iterator monsters_itr = monsters.begin(); monsters_itr != monsters.end(); monsters_itr++)	{
			const Monster& monsters_elmt = *monsters_itr;
			nSize += Monster_Serializer::Size(monsters_elmt);
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
		if(false == ErrorCode_Serializer::Store(_buf_, error_code)) { return false; }
		std::memcpy(*_buf_, &width, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, &height, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		std::memcpy(*_buf_, &player_unit_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		if(false == Vector2Int_Serializer::Store(_buf_, start)) { return false; }
		size_t tiles_size = tiles.size();
		std::memcpy(*_buf_, &tiles_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::vector<DungeonTileType >::const_iterator tiles_itr = tiles.begin(); tiles_itr != tiles.end(); tiles_itr++)	{
			const DungeonTileType& tiles_elmt = *tiles_itr;
			if(false == DungeonTileType_Serializer::Store(_buf_, tiles_elmt)) { return false; }
		}
		size_t monsters_size = monsters.size();
		std::memcpy(*_buf_, &monsters_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::vector<Monster >::const_iterator monsters_itr = monsters.begin(); monsters_itr != monsters.end(); monsters_itr++)	{
			const Monster& monsters_elmt = *monsters_itr;
			if(false == Monster_Serializer::Store(_buf_, monsters_elmt)) { return false; }
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
		if(false == ErrorCode_Serializer::Load(error_code, _buf_, nSize)) { return false; }
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&width, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		if(sizeof(int32_t) > nSize) { return false; }	std::memcpy(&height, *_buf_, sizeof(int32_t));	(*_buf_) += sizeof(int32_t); nSize -= sizeof(int32_t);
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&player_unit_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(false == Vector2Int_Serializer::Load(start, _buf_, nSize)) { return false; }
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t tiles_length = 0; std::memcpy(&tiles_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<tiles_length; i++) {
			DungeonTileType tiles_val;
			if(false == DungeonTileType_Serializer::Load(tiles_val, _buf_, nSize)) { return false; }
			tiles.push_back(tiles_val);
		}
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t monsters_length = 0; std::memcpy(&monsters_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<monsters_length; i++) {
			Monster monsters_val;
			if(false == Monster_Serializer::Load(monsters_val, _buf_, nSize)) { return false; }
			monsters.push_back(monsters_val);
		}
		return true;
	}
}; //MsgSvrCli_CreateDungeon_Ans
struct MsgSvrCli_CreateDungeon_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_CreateDungeon_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_CreateDungeon_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_CreateDungeon_Ans& obj) { return obj.Size(); }
};
struct MsgCliSvr_PlayerMove_Ntf {
	enum { MSG_ID = 40000002 }; 
	Vector2Int	destination;
	MsgCliSvr_PlayerMove_Ntf()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += Vector2Int_Serializer::Size(destination);
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
		if(false == Vector2Int_Serializer::Store(_buf_, destination)) { return false; }
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
		if(false == Vector2Int_Serializer::Load(destination, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgCliSvr_PlayerMove_Ntf
struct MsgCliSvr_PlayerMove_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_PlayerMove_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_PlayerMove_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_PlayerMove_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_UnitPosition_Ntf {
	enum { MSG_ID = 40000003 }; 
	uint64_t	unit_seq;
	Vector2Int	position;
	MsgSvrCli_UnitPosition_Ntf()	{
		unit_seq = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint64_t);
		nSize += Vector2Int_Serializer::Size(position);
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
		std::memcpy(*_buf_, &unit_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		if(false == Vector2Int_Serializer::Store(_buf_, position)) { return false; }
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&unit_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(false == Vector2Int_Serializer::Load(position, _buf_, nSize)) { return false; }
		return true;
	}
}; //MsgSvrCli_UnitPosition_Ntf
struct MsgSvrCli_UnitPosition_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_UnitPosition_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_UnitPosition_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_UnitPosition_Ntf& obj) { return obj.Size(); }
};
struct MsgSvrCli_UnitCreate_Ntf {
	enum { MSG_ID = 40000004 }; 
	uint64_t	unit_seq;
	uint32_t	unit_index;
	MsgSvrCli_UnitCreate_Ntf()	{
		unit_seq = 0;
		unit_index = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += sizeof(uint64_t);
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
		std::memcpy(*_buf_, &unit_seq, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
		std::memcpy(*_buf_, &unit_index, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t);
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
		if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&unit_seq, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
		if(sizeof(uint32_t) > nSize) { return false; }	std::memcpy(&unit_index, *_buf_, sizeof(uint32_t));	(*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		return true;
	}
}; //MsgSvrCli_UnitCreate_Ntf
struct MsgSvrCli_UnitCreate_Ntf_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_UnitCreate_Ntf& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_UnitCreate_Ntf& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_UnitCreate_Ntf& obj) { return obj.Size(); }
};

}}

#endif // __MessageDungeon_H__
