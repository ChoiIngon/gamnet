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
	public ulong	player_unit_seq = 0;
	public Vector2Int	start = new Vector2Int();
	public List<DungeonTileType >	tiles = new List<DungeonTileType >();
	public List<Monster >	monsters = new List<Monster >();
	public MsgSvrCli_CreateDungeon_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
			nSize += sizeof(int);
			nSize += sizeof(int);
			nSize += sizeof(ulong);
			nSize += Vector2Int_Serializer.Size(start);
			nSize += sizeof(int);
			foreach(var tiles_itr in tiles) { 
				DungeonTileType tiles_elmt = tiles_itr;
				nSize += DungeonTileType_Serializer.Size(tiles_elmt);
			}
			nSize += sizeof(int);
			foreach(var monsters_itr in monsters) { 
				Monster monsters_elmt = monsters_itr;
				nSize += Monster_Serializer.Size(monsters_elmt);
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
			_buf_.Write(BitConverter.GetBytes(player_unit_seq), 0, sizeof(ulong));
			if(false == Vector2Int_Serializer.Store(_buf_, start)) { return false; }
			_buf_.Write(BitConverter.GetBytes(tiles.Count), 0, sizeof(int));
			foreach(var tiles_itr in tiles) { 
				DungeonTileType tiles_elmt = tiles_itr;
				if(false == DungeonTileType_Serializer.Store(_buf_, tiles_elmt)) { return false; }
			}
			_buf_.Write(BitConverter.GetBytes(monsters.Count), 0, sizeof(int));
			foreach(var monsters_itr in monsters) { 
				Monster monsters_elmt = monsters_itr;
				if(false == Monster_Serializer.Store(_buf_, monsters_elmt)) { return false; }
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
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			player_unit_seq = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
			if(false == Vector2Int_Serializer.Load(ref start, _buf_)) { return false; }
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int tiles_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int tiles_itr=0; tiles_itr<tiles_length; tiles_itr++) {
				DungeonTileType tiles_val = new DungeonTileType();
				if(false == DungeonTileType_Serializer.Load(ref tiles_val, _buf_)) { return false; }
				tiles.Add(tiles_val);
			}
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int monsters_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int monsters_itr=0; monsters_itr<monsters_length; monsters_itr++) {
				Monster monsters_val = new Monster();
				if(false == Monster_Serializer.Load(ref monsters_val, _buf_)) { return false; }
				monsters.Add(monsters_val);
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
public class MsgCliSvr_PlayerMove_Ntf {
	public const int MSG_ID = 40000002;
	public Vector2Int	destination = new Vector2Int();
	public MsgCliSvr_PlayerMove_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += Vector2Int_Serializer.Size(destination);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == Vector2Int_Serializer.Store(_buf_, destination)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == Vector2Int_Serializer.Load(ref destination, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_PlayerMove_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_PlayerMove_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_PlayerMove_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_PlayerMove_Ntf obj) { return obj.Size(); }
};
public class MsgSvrCli_UnitPosition_Ntf {
	public const int MSG_ID = 40000003;
	public ulong	unit_seq = 0;
	public Vector2Int	position = new Vector2Int();
	public MsgSvrCli_UnitPosition_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(ulong);
			nSize += Vector2Int_Serializer.Size(position);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(unit_seq), 0, sizeof(ulong));
			if(false == Vector2Int_Serializer.Store(_buf_, position)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			unit_seq = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
			if(false == Vector2Int_Serializer.Load(ref position, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_UnitPosition_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_UnitPosition_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_UnitPosition_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_UnitPosition_Ntf obj) { return obj.Size(); }
};
public class MsgSvrCli_UnitCreate_Ntf {
	public const int MSG_ID = 40000004;
	public ulong	unit_seq = 0;
	public uint	unit_index = 0;
	public MsgSvrCli_UnitCreate_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(ulong);
			nSize += sizeof(uint);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(unit_seq), 0, sizeof(ulong));
			_buf_.Write(BitConverter.GetBytes(unit_index), 0, sizeof(uint));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			unit_seq = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			unit_index = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_UnitCreate_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_UnitCreate_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_UnitCreate_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_UnitCreate_Ntf obj) { return obj.Size(); }
};

}}

