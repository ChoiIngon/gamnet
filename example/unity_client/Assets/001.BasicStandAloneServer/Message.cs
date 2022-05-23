using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
public class MsgCliSvr_HelloWorld_Req {
	public const int MSG_ID = 1;
	public string	greeting = "";
	public MsgCliSvr_HelloWorld_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != greeting) { nSize += Encoding.UTF8.GetByteCount(greeting); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != greeting) {
				int greeting_length = Encoding.UTF8.GetByteCount(greeting);
				_buf_.Write(BitConverter.GetBytes(greeting_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(greeting), 0, greeting_length);
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
			int greeting_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(greeting_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] greeting_buf = new byte[greeting_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, greeting_buf, 0, greeting_length);
			greeting = System.Text.Encoding.UTF8.GetString(greeting_buf);
			_buf_.Position += greeting_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_HelloWorld_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_HelloWorld_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_HelloWorld_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_HelloWorld_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_HelloWorld_Ans {
	public const int MSG_ID = 1;
	public string	answer = "";
	public MsgSvrCli_HelloWorld_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != answer) { nSize += Encoding.UTF8.GetByteCount(answer); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != answer) {
				int answer_length = Encoding.UTF8.GetByteCount(answer);
				_buf_.Write(BitConverter.GetBytes(answer_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(answer), 0, answer_length);
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
			int answer_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(answer_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] answer_buf = new byte[answer_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, answer_buf, 0, answer_length);
			answer = System.Text.Encoding.UTF8.GetString(answer_buf);
			_buf_.Position += answer_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_HelloWorld_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_HelloWorld_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_HelloWorld_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_HelloWorld_Ans obj) { return obj.Size(); }
};
public class MsgSvrCli_Welcome_Ntf {
	public const int MSG_ID = 2;
	public string	greeting = "";
	public MsgSvrCli_Welcome_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int); 
			if(null != greeting) { nSize += Encoding.UTF8.GetByteCount(greeting); }
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(null != greeting) {
				int greeting_length = Encoding.UTF8.GetByteCount(greeting);
				_buf_.Write(BitConverter.GetBytes(greeting_length), 0, sizeof(int));
				_buf_.Write(Encoding.UTF8.GetBytes(greeting), 0, greeting_length);
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
			int greeting_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			if(greeting_length > _buf_.Length - _buf_.Position) { return false; }
			byte[] greeting_buf = new byte[greeting_length];
			Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, greeting_buf, 0, greeting_length);
			greeting = System.Text.Encoding.UTF8.GetString(greeting_buf);
			_buf_.Position += greeting_length;
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_Welcome_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Welcome_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Welcome_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Welcome_Ntf obj) { return obj.Size(); }
};
