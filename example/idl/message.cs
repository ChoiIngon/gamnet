using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
namespace message{

//
//
//

public class ItemData {
	public string	item_id = "";
	public uint	item_seq = 0;
	public ItemData() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != item_id) { nSize += Encoding.UTF8.GetByteCount(item_id); }
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
	public string	access_token = "";
	public uint	msg_seq = 0;
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
			if(null != access_token) { nSize += Encoding.UTF8.GetByteCount(access_token); }
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
			if(null != access_token) {
				int access_token_length = Encoding.UTF8.GetByteCount(access_token);
				_buf_.Write(BitConverter.GetBytes(access_token_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(access_token), 0, access_token_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
			_buf_.Write(BitConverter.GetBytes(msg_seq), 0, sizeof(uint));
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
			int access_token_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(access_token_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] access_token_buf = new byte[access_token_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, access_token_buf, 0, access_token_length);
			access_token = System.Text.Encoding.UTF8.GetString(access_token_buf);
			_buf_.Position += access_token_length;
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			msg_seq = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
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
public class MsgCliSvr_Login_Req {
	public const int MSG_ID = 10000001;
	public string	user_id = "";
	public string	access_token = "";
	public MsgCliSvr_Login_Req() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != user_id) { nSize += Encoding.UTF8.GetByteCount(user_id); }
			nSize += sizeof(int); 
			if(null != access_token) { nSize += Encoding.UTF8.GetByteCount(access_token); }
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
			if(null != access_token) {
				int access_token_length = Encoding.UTF8.GetByteCount(access_token);
				_buf_.Write(BitConverter.GetBytes(access_token_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(access_token), 0, access_token_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
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
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int access_token_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(access_token_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] access_token_buf = new byte[access_token_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, access_token_buf, 0, access_token_length);
			access_token = System.Text.Encoding.UTF8.GetString(access_token_buf);
			_buf_.Position += access_token_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_Login_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_Login_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_Login_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_Login_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_Login_Ans {
	public const int MSG_ID = 10000001;
	public uint	error_code = 0;
	public UserData	user_data = new UserData();
	public MsgSvrCli_Login_Ans() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(uint);
			nSize += UserData_Serializer.Size(user_data);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(error_code), 0, sizeof(uint));
			if(false == UserData_Serializer.Store(_buf_, user_data)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			error_code = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
			if(false == UserData_Serializer.Load(ref user_data, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_Login_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Login_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Login_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Login_Ans obj) { return obj.Size(); }
};
public class MsgCliSvr_Move_Ntf {
	public const int MSG_ID = 10000002;
	public uint	seq = 0;
	public float	x = 0.0f;
	public float	y = 0.0f;
	public MsgCliSvr_Move_Ntf() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(uint);
			nSize += sizeof(float);
			nSize += sizeof(float);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(seq), 0, sizeof(uint));
			_buf_.Write(BitConverter.GetBytes(x), 0, sizeof(float));
			_buf_.Write(BitConverter.GetBytes(y), 0, sizeof(float));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			seq = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
			if(sizeof(float) > _buf_.Length - _buf_.Position) { return false; }
			x = BitConverter.ToSingle(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(float);
			if(sizeof(float) > _buf_.Length - _buf_.Position) { return false; }
			y = BitConverter.ToSingle(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(float);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_Move_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_Move_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_Move_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_Move_Ntf obj) { return obj.Size(); }
};
}
