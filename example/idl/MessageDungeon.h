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
	Vector2Int	start;
	std::vector<DungeonTileType >	tiles;
	std::vector<Monster >	monsters;
	MsgSvrCli_CreateDungeon_Ans()	{
		width = 0;
		height = 0;
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(error_code);
		nSize += sizeof(int32_t);
		nSize += sizeof(int32_t);
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
struct MsgCliSvr_PlayerMove_Req {
	enum { MSG_ID = 40000002 }; 
	Vector2Int	destination;
	MsgCliSvr_PlayerMove_Req()	{
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
}; //MsgCliSvr_PlayerMove_Req
struct MsgCliSvr_PlayerMove_Req_Serializer {
	static bool Store(char** _buf_, const MsgCliSvr_PlayerMove_Req& obj) { return obj.Store(_buf_); }
	static bool Load(MsgCliSvr_PlayerMove_Req& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgCliSvr_PlayerMove_Req& obj) { return obj.Size(); }
};
struct MsgSvrCli_PlayerMove_Ans {
	enum { MSG_ID = 40000002 }; 
	ErrorCode	error_code;
	std::list<Vector2Int >	path;
	std::map<uint64_t, std::list<Vector2Int > >	monster_moves;
	MsgSvrCli_PlayerMove_Ans()	{
	}
	size_t Size() const {
		size_t nSize = 0;
		nSize += ErrorCode_Serializer::Size(error_code);
		nSize += sizeof(int32_t);
		for(std::list<Vector2Int >::const_iterator path_itr = path.begin(); path_itr != path.end(); path_itr++)	{
			const Vector2Int& path_elmt = *path_itr;
			nSize += Vector2Int_Serializer::Size(path_elmt);
		}
		nSize += sizeof(uint32_t);
		for(std::map<uint64_t, std::list<Vector2Int > >::const_iterator monster_moves_itr = monster_moves.begin(); monster_moves_itr != monster_moves.end(); monster_moves_itr++) {
			const std::list<Vector2Int >& monster_moves_elmt = monster_moves_itr->second;
			nSize += sizeof(uint64_t);
			nSize += sizeof(int32_t);
			for(std::list<Vector2Int >::const_iterator monster_moves_elmt_itr = monster_moves_elmt.begin(); monster_moves_elmt_itr != monster_moves_elmt.end(); monster_moves_elmt_itr++)	{
				const Vector2Int& monster_moves_elmt_elmt = *monster_moves_elmt_itr;
				nSize += Vector2Int_Serializer::Size(monster_moves_elmt_elmt);
			}
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
		size_t path_size = path.size();
		std::memcpy(*_buf_, &path_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::list<Vector2Int >::const_iterator path_itr = path.begin(); path_itr != path.end(); path_itr++)	{
			const Vector2Int& path_elmt = *path_itr;
			if(false == Vector2Int_Serializer::Store(_buf_, path_elmt)) { return false; }
		}
		size_t monster_moves_size = monster_moves.size();
		std::memcpy(*_buf_, &monster_moves_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
		for(std::map<uint64_t, std::list<Vector2Int > >::const_iterator monster_moves_itr = monster_moves.begin(); monster_moves_itr != monster_moves.end(); monster_moves_itr++) {
			const uint64_t& monster_moves_key = monster_moves_itr->first;
			const std::list<Vector2Int >& monster_moves_elmt = monster_moves_itr->second;
			std::memcpy(*_buf_, &monster_moves_key, sizeof(uint64_t)); (*_buf_) += sizeof(uint64_t);
			size_t monster_moves_elmt_size = monster_moves_elmt.size();
			std::memcpy(*_buf_, &monster_moves_elmt_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);
			for(std::list<Vector2Int >::const_iterator monster_moves_elmt_itr = monster_moves_elmt.begin(); monster_moves_elmt_itr != monster_moves_elmt.end(); monster_moves_elmt_itr++)	{
				const Vector2Int& monster_moves_elmt_elmt = *monster_moves_elmt_itr;
				if(false == Vector2Int_Serializer::Store(_buf_, monster_moves_elmt_elmt)) { return false; }
			}
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
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t path_length = 0; std::memcpy(&path_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<path_length; i++) {
			Vector2Int path_val;
			if(false == Vector2Int_Serializer::Load(path_val, _buf_, nSize)) { return false; }
			path.push_back(path_val);
		}
		if(sizeof(int32_t) > nSize) { return false; }
		uint32_t monster_moves_length = 0; std::memcpy(&monster_moves_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
		for(uint32_t i=0; i<monster_moves_length; i++) {
			uint64_t monster_moves_key;
			if(sizeof(uint64_t) > nSize) { return false; }	std::memcpy(&monster_moves_key, *_buf_, sizeof(uint64_t));	(*_buf_) += sizeof(uint64_t); nSize -= sizeof(uint64_t);
			std::list<Vector2Int > monster_moves_val;
			if(sizeof(int32_t) > nSize) { return false; }
			uint32_t monster_moves_val_length = 0; std::memcpy(&monster_moves_val_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);
			for(uint32_t i=0; i<monster_moves_val_length; i++) {
				Vector2Int monster_moves_val_val;
				if(false == Vector2Int_Serializer::Load(monster_moves_val_val, _buf_, nSize)) { return false; }
				monster_moves_val.push_back(monster_moves_val_val);
			}
			monster_moves.insert(std::make_pair(monster_moves_key, monster_moves_val));
		}
		return true;
	}
}; //MsgSvrCli_PlayerMove_Ans
struct MsgSvrCli_PlayerMove_Ans_Serializer {
	static bool Store(char** _buf_, const MsgSvrCli_PlayerMove_Ans& obj) { return obj.Store(_buf_); }
	static bool Load(MsgSvrCli_PlayerMove_Ans& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }
	static size_t Size(const MsgSvrCli_PlayerMove_Ans& obj) { return obj.Size(); }
};

}}

#endif // __MessageDungeon_H__
