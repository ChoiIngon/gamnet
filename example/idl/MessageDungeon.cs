using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Message { namespace Dungeon {

public class MsgCliSvr_CreateDungeon_Req {
	public const int MSG_ID = 40000001;
	public MsgCliSvr_CreateDungeon_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_CreateDungeon_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_CreateDungeon_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_CreateDungeon_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_CreateDungeon_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_CreateDungeon_Ans {
	public const int MSG_ID = 40000001;
	public ErrorCode	error_code = new ErrorCode();
	public int	width = 0;
	public int	height = 0;
	public Vector2Int	start = new Vector2Int();
	public List<DungeonTileType >	tiles = new List<DungeonTileType >();
	public MsgSvrCli_CreateDungeon_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
			nSize += sizeof(int);
			nSize += sizeof(int);
			nSize += Vector2Int_Serializer.Size(start);
			nSize += sizeof(int);
			foreach(var tiles_itr in tiles) { 
				DungeonTileType tiles_elmt = tiles_itr;
				nSize += DungeonTileType_Serializer.Size(tiles_elmt);
			}
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
			_buf_.Write(BitConverter.GetBytes(width), 0, sizeof(int));
			_buf_.Write(BitConverter.GetBytes(height), 0, sizeof(int));
			if(false == Vector2Int_Serializer.Store(_buf_, start)) { return false; }
			_buf_.Write(BitConverter.GetBytes(tiles.Count), 0, sizeof(int));
			foreach(var tiles_itr in tiles) { 
				DungeonTileType tiles_elmt = tiles_itr;
				if(false == DungeonTileType_Serializer.Store(_buf_, tiles_elmt)) { return false; }
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			width = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			height = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(false == Vector2Int_Serializer.Load(ref start, _buf_)) { return false; }
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int tiles_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int tiles_itr=0; tiles_itr<tiles_length; tiles_itr++) {
				DungeonTileType tiles_val = new DungeonTileType();
				if(false == DungeonTileType_Serializer.Load(ref tiles_val, _buf_)) { return false; }
				tiles.Add(tiles_val);
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_CreateDungeon_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_CreateDungeon_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_CreateDungeon_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_CreateDungeon_Ans obj) { return obj.Size(); }
};

}}

