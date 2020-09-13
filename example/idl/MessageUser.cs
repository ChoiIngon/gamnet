using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Message { namespace User {

public class MsgCliSvr_Login_Req {
	public const int MSG_ID = 11;
	public string	account_id = "";
	public AccountType	account_type = new AccountType();
	public MsgCliSvr_Login_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != account_id) { nSize += Encoding.UTF8.GetByteCount(account_id); }
			nSize += AccountType_Serializer.Size(account_type);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != account_id) {
				int account_id_length = Encoding.UTF8.GetByteCount(account_id);
				_buf_.Write(BitConverter.GetBytes(account_id_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(account_id), 0, account_id_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
			if(false == AccountType_Serializer.Store(_buf_, account_type)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int account_id_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(account_id_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] account_id_buf = new byte[account_id_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, account_id_buf, 0, account_id_length);
			account_id = System.Text.Encoding.UTF8.GetString(account_id_buf);
			_buf_.Position += account_id_length;
			if(false == AccountType_Serializer.Load(ref account_type, _buf_)) { return false; }
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
	public const int MSG_ID = 12;
	public ErrorCode	error_code = new ErrorCode();
	public UserData	user_data = new UserData();
	public MsgSvrCli_Login_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
			nSize += UserData_Serializer.Size(user_data);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
			if(false == UserData_Serializer.Store(_buf_, user_data)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
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
public class MsgCliSvr_Create_Req {
	public const int MSG_ID = 21;
	public string	account_id = "";
	public AccountType	account_type = new AccountType();
	public string	user_name = "";
	public MsgCliSvr_Create_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != account_id) { nSize += Encoding.UTF8.GetByteCount(account_id); }
			nSize += AccountType_Serializer.Size(account_type);
			nSize += sizeof(int); 
			if(null != user_name) { nSize += Encoding.UTF8.GetByteCount(user_name); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != account_id) {
				int account_id_length = Encoding.UTF8.GetByteCount(account_id);
				_buf_.Write(BitConverter.GetBytes(account_id_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(account_id), 0, account_id_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
			if(false == AccountType_Serializer.Store(_buf_, account_type)) { return false; }
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
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int account_id_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(account_id_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] account_id_buf = new byte[account_id_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, account_id_buf, 0, account_id_length);
			account_id = System.Text.Encoding.UTF8.GetString(account_id_buf);
			_buf_.Position += account_id_length;
			if(false == AccountType_Serializer.Load(ref account_type, _buf_)) { return false; }
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
public struct MsgCliSvr_Create_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_Create_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_Create_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_Create_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_Create_Ans {
	public const int MSG_ID = 22;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_Create_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_Create_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Create_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Create_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Create_Ans obj) { return obj.Size(); }
};
public class MsgSvrCli_Counter_Ntf {
	public const int MSG_ID = 31;
	public List<CounterData >	counter_datas = new List<CounterData >();
	public MsgSvrCli_Counter_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int);
			foreach(var counter_datas_itr in counter_datas) { 
				CounterData counter_datas_elmt = counter_datas_itr;
				nSize += CounterData_Serializer.Size(counter_datas_elmt);
			}
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(counter_datas.Count), 0, sizeof(int));
			foreach(var counter_datas_itr in counter_datas) { 
				CounterData counter_datas_elmt = counter_datas_itr;
				if(false == CounterData_Serializer.Store(_buf_, counter_datas_elmt)) { return false; }
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int counter_datas_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int counter_datas_itr=0; counter_datas_itr<counter_datas_length; counter_datas_itr++) {
				CounterData counter_datas_val = new CounterData();
				if(false == CounterData_Serializer.Load(ref counter_datas_val, _buf_)) { return false; }
				counter_datas.Add(counter_datas_val);
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_Counter_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Counter_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Counter_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Counter_Ntf obj) { return obj.Size(); }
};
public class MsgCliSvr_Delete_Req {
	public const int MSG_ID = 32;
	public MsgCliSvr_Delete_Req() {
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
public struct MsgCliSvr_Delete_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_Delete_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_Delete_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_Delete_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_Delete_Ans {
	public const int MSG_ID = 33;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_Delete_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_Delete_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Delete_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Delete_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Delete_Ans obj) { return obj.Size(); }
};
public class MsgCliSvr_Restore_Req {
	public const int MSG_ID = 34;
	public MsgCliSvr_Restore_Req() {
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
public struct MsgCliSvr_Restore_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_Restore_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_Restore_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_Restore_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_Restore_Ans {
	public const int MSG_ID = 35;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_Restore_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_Restore_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Restore_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Restore_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Restore_Ans obj) { return obj.Size(); }
};

}}

