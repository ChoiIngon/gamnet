using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
namespace data{
public enum ErrorCode {
	Success = 0,
	MessageFormatError = 1000,
	MessageSeqOmmitError = 1001,
	InvalidUserError = 2000,
	InvalidAccessTokenError = 2001,
	DuplicateConnectionError = 3000,
	ReconnectTimeoutError = 4000,
	ResponseTimeoutError = 4001,
	AlreadyLoginSessionError = 5000,
}; // ErrorCode
public struct ErrorCode_Serializer {
	public static bool Store(System.IO.MemoryStream _buf_, ErrorCode obj) { 
		try {
			_buf_.Write(System.BitConverter.GetBytes((int)obj), 0, sizeof(ErrorCode));
		}
		catch(System.Exception) {
			return false;
		}
		return true;
	}
	public static bool Load(ref ErrorCode obj, MemoryStream _buf_) { 
		try {
			obj = (ErrorCode)System.BitConverter.ToInt32(_buf_.ToArray(), (int)_buf_.Position);
			_buf_.Position += sizeof(ErrorCode);
		}
		catch(System.Exception) { 
			return false;
		}
		return true;
	}
	public static System.Int32 Size(ErrorCode obj) { return sizeof(ErrorCode); }
};
public enum ItemType {
	Invalid,
	Weapon,
	Armor,
	Potion,
}; // ItemType
public struct ItemType_Serializer {
	public static bool Store(System.IO.MemoryStream _buf_, ItemType obj) { 
		try {
			_buf_.Write(System.BitConverter.GetBytes((int)obj), 0, sizeof(ItemType));
		}
		catch(System.Exception) {
			return false;
		}
		return true;
	}
	public static bool Load(ref ItemType obj, MemoryStream _buf_) { 
		try {
			obj = (ItemType)System.BitConverter.ToInt32(_buf_.ToArray(), (int)_buf_.Position);
			_buf_.Position += sizeof(ItemType);
		}
		catch(System.Exception) { 
			return false;
		}
		return true;
	}
	public static System.Int32 Size(ItemType obj) { return sizeof(ItemType); }
};
public class ItemData {
	public string	item_id = "";
	public ItemType	item_type = new ItemType();
	public uint	item_seq = 0;
	public ItemData() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != item_id) { nSize += Encoding.UTF8.GetByteCount(item_id); }
			nSize += ItemType_Serializer.Size(item_type);
			nSize += sizeof(uint);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public bool Store(MemoryStream _buf_) {
		try {
			if(null != item_id) {
				int item_id_length = Encoding.UTF8.GetByteCount(item_id);
				_buf_.Write(BitConverter.GetBytes(item_id_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(item_id), 0, item_id_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
			if(false == ItemType_Serializer.Store(_buf_, item_type)) { return false; }
			_buf_.Write(BitConverter.GetBytes(item_seq), 0, sizeof(uint));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int item_id_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(item_id_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] item_id_buf = new byte[item_id_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, item_id_buf, 0, item_id_length);
			item_id = System.Text.Encoding.UTF8.GetString(item_id_buf);
			_buf_.Position += item_id_length;
			if(false == ItemType_Serializer.Load(ref item_type, _buf_)) { return false; }
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			item_seq = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct ItemData_Serializer {
	public static bool Store(MemoryStream _buf_, ItemData obj) { return obj.Store(_buf_); }
	public static bool Load(ref ItemData obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(ItemData obj) { return obj.Size(); }
};
public class UserData {
	public string	user_id = "";
	public uint	user_seq = 0;
	public List<ItemData >	items = new List<ItemData >();
	public UserData() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != user_id) { nSize += Encoding.UTF8.GetByteCount(user_id); }
			nSize += sizeof(uint);
			nSize += sizeof(int);
			foreach(var items_itr in items) { 
				ItemData items_elmt = items_itr;
				nSize += ItemData_Serializer.Size(items_elmt);
			}
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public bool Store(MemoryStream _buf_) {
		try {
			if(null != user_id) {
				int user_id_length = Encoding.UTF8.GetByteCount(user_id);
				_buf_.Write(BitConverter.GetBytes(user_id_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(user_id), 0, user_id_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
			_buf_.Write(BitConverter.GetBytes(user_seq), 0, sizeof(uint));
			_buf_.Write(BitConverter.GetBytes(items.Count), 0, sizeof(int));
			foreach(var items_itr in items) { 
				ItemData items_elmt = items_itr;
				if(false == ItemData_Serializer.Store(_buf_, items_elmt)) { return false; }
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int user_id_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(user_id_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] user_id_buf = new byte[user_id_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, user_id_buf, 0, user_id_length);
			user_id = System.Text.Encoding.UTF8.GetString(user_id_buf);
			_buf_.Position += user_id_length;
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			user_seq = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int items_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int items_itr=0; items_itr<items_length; items_itr++) {
				ItemData items_val = new ItemData();
				if(false == ItemData_Serializer.Load(ref items_val, _buf_)) { return false; }
				items.Add(items_val);
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct UserData_Serializer {
	public static bool Store(MemoryStream _buf_, UserData obj) { return obj.Store(_buf_); }
	public static bool Load(ref UserData obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(UserData obj) { return obj.Size(); }
};
}
