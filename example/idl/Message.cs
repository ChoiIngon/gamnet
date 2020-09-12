using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
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
public class UserData {
	public string	user_id = "";
	public uint	user_seq = 0;
	public uint	frame = 0;
	public UserData() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != user_id) { nSize += Encoding.UTF8.GetByteCount(user_id); }
			nSize += sizeof(uint);
			nSize += sizeof(uint);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
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
			_buf_.Write(BitConverter.GetBytes(frame), 0, sizeof(uint));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
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
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			frame = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
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
	public const int MSG_ID = 11;
	public string	user_id = "";
	public MsgCliSvr_Login_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != user_id) { nSize += Encoding.UTF8.GetByteCount(user_id); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != user_id) {
				int user_id_length = Encoding.UTF8.GetByteCount(user_id);
				_buf_.Write(BitConverter.GetBytes(user_id_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(user_id), 0, user_id_length);
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
			int user_id_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(user_id_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] user_id_buf = new byte[user_id_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, user_id_buf, 0, user_id_length);
			user_id = System.Text.Encoding.UTF8.GetString(user_id_buf);
			_buf_.Position += user_id_length;
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
public class MsgCliSvr_UpdateFrame_Req {
	public const int MSG_ID = 21;
	public MsgCliSvr_UpdateFrame_Req() {
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
public struct MsgCliSvr_UpdateFrame_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_UpdateFrame_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_UpdateFrame_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_UpdateFrame_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_UpdateFrame_Ans {
	public const int MSG_ID = 22;
	public ErrorCode	error_code = new ErrorCode();
	public uint	frame = 0;
	public MsgSvrCli_UpdateFrame_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
			nSize += sizeof(uint);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
			_buf_.Write(BitConverter.GetBytes(frame), 0, sizeof(uint));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			frame = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_UpdateFrame_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_UpdateFrame_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_UpdateFrame_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_UpdateFrame_Ans obj) { return obj.Size(); }
};
public class MsgCliSvr_JoinChannel_Req {
	public const int MSG_ID = 31;
	public MsgCliSvr_JoinChannel_Req() {
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
public struct MsgCliSvr_JoinChannel_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_JoinChannel_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_JoinChannel_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_JoinChannel_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_JoinChannel_Ans {
	public const int MSG_ID = 32;
	public ErrorCode	error_code = new ErrorCode();
	public uint	channel_seq = 0;
	public List<UserData >	user_datas = new List<UserData >();
	public MsgSvrCli_JoinChannel_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
			nSize += sizeof(uint);
			nSize += sizeof(int);
			foreach(var user_datas_itr in user_datas) { 
				UserData user_datas_elmt = user_datas_itr;
				nSize += UserData_Serializer.Size(user_datas_elmt);
			}
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
			_buf_.Write(BitConverter.GetBytes(channel_seq), 0, sizeof(uint));
			_buf_.Write(BitConverter.GetBytes(user_datas.Count), 0, sizeof(int));
			foreach(var user_datas_itr in user_datas) { 
				UserData user_datas_elmt = user_datas_itr;
				if(false == UserData_Serializer.Store(_buf_, user_datas_elmt)) { return false; }
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			channel_seq = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int user_datas_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int user_datas_itr=0; user_datas_itr<user_datas_length; user_datas_itr++) {
				UserData user_datas_val = new UserData();
				if(false == UserData_Serializer.Load(ref user_datas_val, _buf_)) { return false; }
				user_datas.Add(user_datas_val);
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_JoinChannel_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_JoinChannel_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_JoinChannel_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_JoinChannel_Ans obj) { return obj.Size(); }
};
public class MsgSvrCli_JoinChannel_Ntf {
	public const int MSG_ID = 33;
	public uint	user_count = 0;
	public UserData	new_user_data = new UserData();
	public MsgSvrCli_JoinChannel_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(uint);
			nSize += UserData_Serializer.Size(new_user_data);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(user_count), 0, sizeof(uint));
			if(false == UserData_Serializer.Store(_buf_, new_user_data)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			user_count = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
			if(false == UserData_Serializer.Load(ref new_user_data, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_JoinChannel_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_JoinChannel_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_JoinChannel_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_JoinChannel_Ntf obj) { return obj.Size(); }
};
public class MsgCliSvr_SendMessage_Req {
	public const int MSG_ID = 41;
	public string	text = "";
	public MsgCliSvr_SendMessage_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != text) { nSize += Encoding.UTF8.GetByteCount(text); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != text) {
				int text_length = Encoding.UTF8.GetByteCount(text);
				_buf_.Write(BitConverter.GetBytes(text_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(text), 0, text_length);
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
			int text_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(text_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] text_buf = new byte[text_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, text_buf, 0, text_length);
			text = System.Text.Encoding.UTF8.GetString(text_buf);
			_buf_.Position += text_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_SendMessage_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_SendMessage_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_SendMessage_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_SendMessage_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_SendMessage_Ans {
	public const int MSG_ID = 42;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_SendMessage_Ans() {
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
public struct MsgSvrCli_SendMessage_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_SendMessage_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_SendMessage_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_SendMessage_Ans obj) { return obj.Size(); }
};
public class MsgSvrSvr_SendMessage_Req {
	public const int MSG_ID = 43;
	public string	text = "";
	public MsgSvrSvr_SendMessage_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != text) { nSize += Encoding.UTF8.GetByteCount(text); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != text) {
				int text_length = Encoding.UTF8.GetByteCount(text);
				_buf_.Write(BitConverter.GetBytes(text_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(text), 0, text_length);
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
			int text_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(text_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] text_buf = new byte[text_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, text_buf, 0, text_length);
			text = System.Text.Encoding.UTF8.GetString(text_buf);
			_buf_.Position += text_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrSvr_SendMessage_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrSvr_SendMessage_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrSvr_SendMessage_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrSvr_SendMessage_Req obj) { return obj.Size(); }
};
public class MsgSvrSvr_SendMessage_Ans {
	public const int MSG_ID = 44;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrSvr_SendMessage_Ans() {
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
public struct MsgSvrSvr_SendMessage_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrSvr_SendMessage_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrSvr_SendMessage_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrSvr_SendMessage_Ans obj) { return obj.Size(); }
};
public class MsgCliSvr_SendMessage_Ntf {
	public const int MSG_ID = 45;
	public string	text = "";
	public MsgCliSvr_SendMessage_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != text) { nSize += Encoding.UTF8.GetByteCount(text); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != text) {
				int text_length = Encoding.UTF8.GetByteCount(text);
				_buf_.Write(BitConverter.GetBytes(text_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(text), 0, text_length);
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
			int text_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(text_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] text_buf = new byte[text_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, text_buf, 0, text_length);
			text = System.Text.Encoding.UTF8.GetString(text_buf);
			_buf_.Position += text_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_SendMessage_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_SendMessage_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_SendMessage_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_SendMessage_Ntf obj) { return obj.Size(); }
};
public class MsgSvrCli_SendMessage_Ntf {
	public const int MSG_ID = 46;
	public string	text = "";
	public MsgSvrCli_SendMessage_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != text) { nSize += Encoding.UTF8.GetByteCount(text); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != text) {
				int text_length = Encoding.UTF8.GetByteCount(text);
				_buf_.Write(BitConverter.GetBytes(text_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(text), 0, text_length);
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
			int text_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(text_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] text_buf = new byte[text_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, text_buf, 0, text_length);
			text = System.Text.Encoding.UTF8.GetString(text_buf);
			_buf_.Position += text_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_SendMessage_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_SendMessage_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_SendMessage_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_SendMessage_Ntf obj) { return obj.Size(); }
};
public class MsgSvrSvr_SendMessage_Ntf {
	public const int MSG_ID = 47;
	public string	text = "";
	public MsgSvrSvr_SendMessage_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != text) { nSize += Encoding.UTF8.GetByteCount(text); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != text) {
				int text_length = Encoding.UTF8.GetByteCount(text);
				_buf_.Write(BitConverter.GetBytes(text_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(text), 0, text_length);
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
			int text_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(text_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] text_buf = new byte[text_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, text_buf, 0, text_length);
			text = System.Text.Encoding.UTF8.GetString(text_buf);
			_buf_.Position += text_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrSvr_SendMessage_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrSvr_SendMessage_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrSvr_SendMessage_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrSvr_SendMessage_Ntf obj) { return obj.Size(); }
};
public class MsgCliSvr_LeaveChannel_Req {
	public const int MSG_ID = 51;
	public MsgCliSvr_LeaveChannel_Req() {
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
public struct MsgCliSvr_LeaveChannel_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_LeaveChannel_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_LeaveChannel_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_LeaveChannel_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_LeaveChannel_Ans {
	public const int MSG_ID = 52;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_LeaveChannel_Ans() {
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
public struct MsgSvrCli_LeaveChannel_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_LeaveChannel_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_LeaveChannel_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_LeaveChannel_Ans obj) { return obj.Size(); }
};
public class MsgSvrCli_LeaveChannel_Ntf {
	public const int MSG_ID = 53;
	public UserData	leave_user_data = new UserData();
	public MsgSvrCli_LeaveChannel_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += UserData_Serializer.Size(leave_user_data);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == UserData_Serializer.Store(_buf_, leave_user_data)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == UserData_Serializer.Load(ref leave_user_data, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_LeaveChannel_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_LeaveChannel_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_LeaveChannel_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_LeaveChannel_Ntf obj) { return obj.Size(); }
};
public class MsgSvrCli_Kickout_Ntf {
	public const int MSG_ID = 61;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_Kickout_Ntf() {
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
public struct MsgSvrCli_Kickout_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Kickout_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Kickout_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Kickout_Ntf obj) { return obj.Size(); }
};
public class MsgCliSvr_HeartBeat_Ntf {
	public const int MSG_ID = 71;
	public int	msg_seq = 0;
	public MsgCliSvr_HeartBeat_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(msg_seq), 0, sizeof(int));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			msg_seq = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_HeartBeat_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_HeartBeat_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_HeartBeat_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_HeartBeat_Ntf obj) { return obj.Size(); }
};
public class MsgSvrCli_HeartBeat_Ntf {
	public const int MSG_ID = 72;
	public int	msg_seq = 0;
	public MsgSvrCli_HeartBeat_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(msg_seq), 0, sizeof(int));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			msg_seq = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_HeartBeat_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_HeartBeat_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_HeartBeat_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_HeartBeat_Ntf obj) { return obj.Size(); }
};
