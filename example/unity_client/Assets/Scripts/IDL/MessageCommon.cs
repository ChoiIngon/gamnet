using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Message { 

public enum ErrorCode {
	Success = 0,
	MessageFormatError = 1000,
	MessageSeqOmmitError = 1001,
	InvalidUserError = 2000,
	InvalidAccessTokenError = 2001,
	InvalidSeviceName = 2002,
	DuplicateConnectionError = 3000,
	ReconnectTimeoutError = 4000,
	ResponseTimeoutError = 4001,
	AlreadyLoginSessionError = 5000,
	CanNotCreateCastGroup = 5001,
	DuplicateNameError,
	CreateAccountError,
	UndefineError = 99999,
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
public enum AccountType {
	Invalid,
	Dev = 1,
	Google = 2,
	Apple = 3,
	Facebook = 4,
}; // AccountType
public struct AccountType_Serializer {
	public static bool Store(System.IO.MemoryStream _buf_, AccountType obj) { 
		try {
			_buf_.Write(System.BitConverter.GetBytes((int)obj), 0, sizeof(AccountType));
		}
		catch(System.Exception) {
			return false;
		}
		return true;
	}
	public static bool Load(ref AccountType obj, MemoryStream _buf_) { 
		try {
			obj = (AccountType)System.BitConverter.ToInt32(_buf_.ToArray(), (int)_buf_.Position);
			_buf_.Position += sizeof(AccountType);
		}
		catch(System.Exception) { 
			return false;
		}
		return true;
	}
	public static System.Int32 Size(AccountType obj) { return sizeof(AccountType); }
};
public enum AccountState {
	Invalid,
	Normal,
	Penalty,
	OnDelete,
	Deleted,
}; // AccountState
public struct AccountState_Serializer {
	public static bool Store(System.IO.MemoryStream _buf_, AccountState obj) { 
		try {
			_buf_.Write(System.BitConverter.GetBytes((int)obj), 0, sizeof(AccountState));
		}
		catch(System.Exception) {
			return false;
		}
		return true;
	}
	public static bool Load(ref AccountState obj, MemoryStream _buf_) { 
		try {
			obj = (AccountState)System.BitConverter.ToInt32(_buf_.ToArray(), (int)_buf_.Position);
			_buf_.Position += sizeof(AccountState);
		}
		catch(System.Exception) { 
			return false;
		}
		return true;
	}
	public static System.Int32 Size(AccountState obj) { return sizeof(AccountState); }
};
public enum CounterType {
	Invalid = 0,
	Gold = 1,
	Cash = 2,
}; // CounterType
public struct CounterType_Serializer {
	public static bool Store(System.IO.MemoryStream _buf_, CounterType obj) { 
		try {
			_buf_.Write(System.BitConverter.GetBytes((int)obj), 0, sizeof(CounterType));
		}
		catch(System.Exception) {
			return false;
		}
		return true;
	}
	public static bool Load(ref CounterType obj, MemoryStream _buf_) { 
		try {
			obj = (CounterType)System.BitConverter.ToInt32(_buf_.ToArray(), (int)_buf_.Position);
			_buf_.Position += sizeof(CounterType);
		}
		catch(System.Exception) { 
			return false;
		}
		return true;
	}
	public static System.Int32 Size(CounterType obj) { return sizeof(CounterType); }
};
public enum ItemType {
	Invalid,
	Equip,
	Package,
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
public enum DungeonTileType {
	Invalid,
	Floor,
	Wall,
	Door,
	StairDown,
	StairUp,
}; // DungeonTileType
public struct DungeonTileType_Serializer {
	public static bool Store(System.IO.MemoryStream _buf_, DungeonTileType obj) { 
		try {
			_buf_.Write(System.BitConverter.GetBytes((int)obj), 0, sizeof(DungeonTileType));
		}
		catch(System.Exception) {
			return false;
		}
		return true;
	}
	public static bool Load(ref DungeonTileType obj, MemoryStream _buf_) { 
		try {
			obj = (DungeonTileType)System.BitConverter.ToInt32(_buf_.ToArray(), (int)_buf_.Position);
			_buf_.Position += sizeof(DungeonTileType);
		}
		catch(System.Exception) { 
			return false;
		}
		return true;
	}
	public static System.Int32 Size(DungeonTileType obj) { return sizeof(DungeonTileType); }
};
public class UserData {
	public ulong	user_seq = 0;
	public string	user_name = "";
	public UserData() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(ulong);
			nSize += sizeof(int); 
			if(null != user_name) { nSize += Encoding.UTF8.GetByteCount(user_name); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(user_seq), 0, sizeof(ulong));
			if(null != user_name) {
				int user_name_length = Encoding.UTF8.GetByteCount(user_name);
				_buf_.Write(BitConverter.GetBytes(user_name_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(user_name), 0, user_name_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			user_seq = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int user_name_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(user_name_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] user_name_buf = new byte[user_name_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, user_name_buf, 0, user_name_length);
			user_name = System.Text.Encoding.UTF8.GetString(user_name_buf);
			_buf_.Position += user_name_length;
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
public class MailData {
	public ulong	mail_seq = 0;
	public string	mail_message = "";
	public ulong	expire_date = 0;
	public uint	item_id = 0;
	public uint	item_count = 0;
	public MailData() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(ulong);
			nSize += sizeof(int); 
			if(null != mail_message) { nSize += Encoding.UTF8.GetByteCount(mail_message); }
			nSize += sizeof(ulong);
			nSize += sizeof(uint);
			nSize += sizeof(uint);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(mail_seq), 0, sizeof(ulong));
			if(null != mail_message) {
				int mail_message_length = Encoding.UTF8.GetByteCount(mail_message);
				_buf_.Write(BitConverter.GetBytes(mail_message_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(mail_message), 0, mail_message_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
			_buf_.Write(BitConverter.GetBytes(expire_date), 0, sizeof(ulong));
			_buf_.Write(BitConverter.GetBytes(item_id), 0, sizeof(uint));
			_buf_.Write(BitConverter.GetBytes(item_count), 0, sizeof(uint));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			mail_seq = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int mail_message_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(mail_message_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] mail_message_buf = new byte[mail_message_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, mail_message_buf, 0, mail_message_length);
			mail_message = System.Text.Encoding.UTF8.GetString(mail_message_buf);
			_buf_.Position += mail_message_length;
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			expire_date = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			item_id = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			item_count = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MailData_Serializer {
	public static bool Store(MemoryStream _buf_, MailData obj) { return obj.Store(_buf_); }
	public static bool Load(ref MailData obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MailData obj) { return obj.Size(); }
};
public class CounterData {
	public CounterType	counter_id = new CounterType();
	public ulong	update_date = 0;
	public int	count = 0;
	public CounterData() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += CounterType_Serializer.Size(counter_id);
			nSize += sizeof(ulong);
			nSize += sizeof(int);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == CounterType_Serializer.Store(_buf_, counter_id)) { return false; }
			_buf_.Write(BitConverter.GetBytes(update_date), 0, sizeof(ulong));
			_buf_.Write(BitConverter.GetBytes(count), 0, sizeof(int));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == CounterType_Serializer.Load(ref counter_id, _buf_)) { return false; }
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			update_date = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			count = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct CounterData_Serializer {
	public static bool Store(MemoryStream _buf_, CounterData obj) { return obj.Store(_buf_); }
	public static bool Load(ref CounterData obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(CounterData obj) { return obj.Size(); }
};
public class ItemData {
	public ulong	item_seq = 0;
	public uint	item_id = 0;
	public ItemType	item_type = new ItemType();
	public int	item_count = 0;
	public ItemData() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(ulong);
			nSize += sizeof(uint);
			nSize += ItemType_Serializer.Size(item_type);
			nSize += sizeof(int);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(item_seq), 0, sizeof(ulong));
			_buf_.Write(BitConverter.GetBytes(item_id), 0, sizeof(uint));
			if(false == ItemType_Serializer.Store(_buf_, item_type)) { return false; }
			_buf_.Write(BitConverter.GetBytes(item_count), 0, sizeof(int));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			item_seq = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			item_id = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
			if(false == ItemType_Serializer.Load(ref item_type, _buf_)) { return false; }
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			item_count = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
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
public class Vector2Int {
	public int	x = 0;
	public int	y = 0;
	public Vector2Int() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int);
			nSize += sizeof(int);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(x), 0, sizeof(int));
			_buf_.Write(BitConverter.GetBytes(y), 0, sizeof(int));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			x = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			y = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct Vector2Int_Serializer {
	public static bool Store(MemoryStream _buf_, Vector2Int obj) { return obj.Store(_buf_); }
	public static bool Load(ref Vector2Int obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(Vector2Int obj) { return obj.Size(); }
};

}

