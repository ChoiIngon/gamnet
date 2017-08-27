using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
namespace message{

using data;

public class MsgCliSvr_Login_Req {
	public const int MSG_ID = 10000001;
	public string	user_id = "";
	public MsgCliSvr_Login_Req() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != user_id) { nSize += Encoding.UTF8.GetByteCount(user_id); }
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
	public ErrorCode	error_code = new ErrorCode();
	public UserData	user_data = new UserData();
	public MsgSvrCli_Login_Ans() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
			nSize += UserData_Serializer.Size(user_data);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
			if(false == UserData_Serializer.Store(_buf_, user_data)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public bool Load(MemoryStream _buf_) {
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
public class MsgSvrCli_Kickout_Ntf {
	public const int MSG_ID = 10000003;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_Kickout_Ntf() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public bool Load(MemoryStream _buf_) {
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
	public uint	msg_seq = 0;
	public MsgCliSvr_HeartBeat_Ntf() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(uint);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(msg_seq), 0, sizeof(uint));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			msg_seq = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
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
	public uint	msg_seq = 0;
	public MsgSvrCli_HeartBeat_Ntf() {
	}
	public int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(uint);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(msg_seq), 0, sizeof(uint));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(uint) > _buf_.Length - _buf_.Position) { return false; }
			msg_seq = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(uint);
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
