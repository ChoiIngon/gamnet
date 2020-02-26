using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
namespace Message{
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
	public string	UserID = "";
	public uint	UserSEQ = 0;
	public UserData() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != UserID) { nSize += Encoding.UTF8.GetByteCount(UserID); }
			nSize += sizeof(uint);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != UserID) {
				int UserID_length = Encoding.UTF8.GetByteCount(UserID);
				_buf_.Write(BitConverter.GetBytes(UserID_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(UserID), 0, UserID_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
			_buf_.Write(BitConverter.GetBytes(UserSEQ), 0, sizeof(uint));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int UserID_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(UserID_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] UserID_buf = new byte[UserID_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, UserID_buf, 0, UserID_length);
			UserID = System.Text.Encoding.UTF8.GetString(UserID_buf);
			_buf_.Position += UserID_length;
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			UserSEQ = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
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
	public const int MSG_ID = 10000001;
	public string	UserID = "";
	public MsgCliSvr_Login_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != UserID) { nSize += Encoding.UTF8.GetByteCount(UserID); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != UserID) {
				int UserID_length = Encoding.UTF8.GetByteCount(UserID);
				_buf_.Write(BitConverter.GetBytes(UserID_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(UserID), 0, UserID_length);
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
			int UserID_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(UserID_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] UserID_buf = new byte[UserID_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, UserID_buf, 0, UserID_length);
			UserID = System.Text.Encoding.UTF8.GetString(UserID_buf);
			_buf_.Position += UserID_length;
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
	public ErrorCode	ErrorCode = new ErrorCode();
	public UserData	UserData = new UserData();
	public MsgSvrCli_Login_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(ErrorCode);
			nSize += UserData_Serializer.Size(UserData);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, ErrorCode)) { return false; }
			if(false == UserData_Serializer.Store(_buf_, UserData)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref ErrorCode, _buf_)) { return false; }
			if(false == UserData_Serializer.Load(ref UserData, _buf_)) { return false; }
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
public class MsgCliSvr_JoinChannel_Req {
	public const int MSG_ID = 10001;
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
	public const int MSG_ID = 10001;
	public ErrorCode	ErrorCode = new ErrorCode();
	public MsgSvrCli_JoinChannel_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(ErrorCode);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, ErrorCode)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref ErrorCode, _buf_)) { return false; }
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
	public const int MSG_ID = 10010001;
	public string	SessionKey = "";
	public int	SessionCount = 0;
	public MsgSvrCli_JoinChannel_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != SessionKey) { nSize += Encoding.UTF8.GetByteCount(SessionKey); }
			nSize += sizeof(int);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != SessionKey) {
				int SessionKey_length = Encoding.UTF8.GetByteCount(SessionKey);
				_buf_.Write(BitConverter.GetBytes(SessionKey_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(SessionKey), 0, SessionKey_length);
			}
			else {
				_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));
			}
			_buf_.Write(BitConverter.GetBytes(SessionCount), 0, sizeof(int));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int SessionKey_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(SessionKey_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] SessionKey_buf = new byte[SessionKey_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, SessionKey_buf, 0, SessionKey_length);
			SessionKey = System.Text.Encoding.UTF8.GetString(SessionKey_buf);
			_buf_.Position += SessionKey_length;
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			SessionCount = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
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
public class MsgCliSvr_SendMessage_Ntf {
	public const int MSG_ID = 1020;
	public string	Message = "";
	public MsgCliSvr_SendMessage_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != Message) { nSize += Encoding.UTF8.GetByteCount(Message); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != Message) {
				int Message_length = Encoding.UTF8.GetByteCount(Message);
				_buf_.Write(BitConverter.GetBytes(Message_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(Message), 0, Message_length);
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
			int Message_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(Message_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] Message_buf = new byte[Message_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, Message_buf, 0, Message_length);
			Message = System.Text.Encoding.UTF8.GetString(Message_buf);
			_buf_.Position += Message_length;
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
	public const int MSG_ID = 1020;
	public string	Message = "";
	public MsgSvrCli_SendMessage_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != Message) { nSize += Encoding.UTF8.GetByteCount(Message); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != Message) {
				int Message_length = Encoding.UTF8.GetByteCount(Message);
				_buf_.Write(BitConverter.GetBytes(Message_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(Message), 0, Message_length);
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
			int Message_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(Message_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] Message_buf = new byte[Message_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, Message_buf, 0, Message_length);
			Message = System.Text.Encoding.UTF8.GetString(Message_buf);
			_buf_.Position += Message_length;
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
public class MsgSvrSvr_SendMessage_Req {
	public const int MSG_ID = 21;
	public string	Message = "";
	public MsgSvrSvr_SendMessage_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != Message) { nSize += Encoding.UTF8.GetByteCount(Message); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != Message) {
				int Message_length = Encoding.UTF8.GetByteCount(Message);
				_buf_.Write(BitConverter.GetBytes(Message_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(Message), 0, Message_length);
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
			int Message_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(Message_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] Message_buf = new byte[Message_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, Message_buf, 0, Message_length);
			Message = System.Text.Encoding.UTF8.GetString(Message_buf);
			_buf_.Position += Message_length;
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
	public const int MSG_ID = 22;
	public string	Message = "";
	public MsgSvrSvr_SendMessage_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != Message) { nSize += Encoding.UTF8.GetByteCount(Message); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != Message) {
				int Message_length = Encoding.UTF8.GetByteCount(Message);
				_buf_.Write(BitConverter.GetBytes(Message_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(Message), 0, Message_length);
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
			int Message_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(Message_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] Message_buf = new byte[Message_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, Message_buf, 0, Message_length);
			Message = System.Text.Encoding.UTF8.GetString(Message_buf);
			_buf_.Position += Message_length;
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
public class MsgSvrSvr_SendMessage_Ntf {
	public const int MSG_ID = 1022;
	public string	Message = "";
	public MsgSvrSvr_SendMessage_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != Message) { nSize += Encoding.UTF8.GetByteCount(Message); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != Message) {
				int Message_length = Encoding.UTF8.GetByteCount(Message);
				_buf_.Write(BitConverter.GetBytes(Message_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(Message), 0, Message_length);
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
			int Message_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(Message_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] Message_buf = new byte[Message_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, Message_buf, 0, Message_length);
			Message = System.Text.Encoding.UTF8.GetString(Message_buf);
			_buf_.Position += Message_length;
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
	public const int MSG_ID = 10003;
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
	public const int MSG_ID = 10003;
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
	public const int MSG_ID = 10010003;
	public string	session_key = "";
	public MsgSvrCli_LeaveChannel_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != session_key) { nSize += Encoding.UTF8.GetByteCount(session_key); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != session_key) {
				int session_key_length = Encoding.UTF8.GetByteCount(session_key);
				_buf_.Write(BitConverter.GetBytes(session_key_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(session_key), 0, session_key_length);
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
			int session_key_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(session_key_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] session_key_buf = new byte[session_key_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, session_key_buf, 0, session_key_length);
			session_key = System.Text.Encoding.UTF8.GetString(session_key_buf);
			_buf_.Position += session_key_length;
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
	public const int MSG_ID = 10000003;
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
	public const int MSG_ID = 10000004;
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
	public const int MSG_ID = 10000004;
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
}
