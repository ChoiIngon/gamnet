using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Message { namespace Lobby {

public class MsgCliSvr_Join_Req {
	public const int MSG_ID = 120000001;
	public MsgCliSvr_Join_Req() {
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
public struct MsgCliSvr_Join_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_Join_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_Join_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_Join_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_Join_Ans {
	public const int MSG_ID = 120000002;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_Join_Ans() {
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
public struct MsgSvrCli_Join_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Join_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Join_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Join_Ans obj) { return obj.Size(); }
};
public class MsgSvrCli_Mail_Ntf {
	public const int MSG_ID = 120000003;
	public List<MailData >	mail_datas = new List<MailData >();
	public MsgSvrCli_Mail_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int);
			foreach(var mail_datas_itr in mail_datas) { 
				MailData mail_datas_elmt = mail_datas_itr;
				nSize += MailData_Serializer.Size(mail_datas_elmt);
			}
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(mail_datas.Count), 0, sizeof(int));
			foreach(var mail_datas_itr in mail_datas) { 
				MailData mail_datas_elmt = mail_datas_itr;
				if(false == MailData_Serializer.Store(_buf_, mail_datas_elmt)) { return false; }
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int mail_datas_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int mail_datas_itr=0; mail_datas_itr<mail_datas_length; mail_datas_itr++) {
				MailData mail_datas_val = new MailData();
				if(false == MailData_Serializer.Load(ref mail_datas_val, _buf_)) { return false; }
				mail_datas.Add(mail_datas_val);
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_Mail_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Mail_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Mail_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Mail_Ntf obj) { return obj.Size(); }
};
public class MsgCliSvr_OpenMail_Req {
	public const int MSG_ID = 120000004;
	public ulong	mail_seq = 0;
	public MsgCliSvr_OpenMail_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(ulong);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(mail_seq), 0, sizeof(ulong));
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
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_OpenMail_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_OpenMail_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_OpenMail_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_OpenMail_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_OpenMail_Ans {
	public const int MSG_ID = 120000005;
	public ErrorCode	error_code = new ErrorCode();
	public ulong	mail_seq = 0;
	public MsgSvrCli_OpenMail_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
			nSize += sizeof(ulong);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
			_buf_.Write(BitConverter.GetBytes(mail_seq), 0, sizeof(ulong));
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
			if(sizeof(ulong) > _buf_.Length - _buf_.Position) { return false; }
			mail_seq = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(ulong);
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_OpenMail_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_OpenMail_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_OpenMail_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_OpenMail_Ans obj) { return obj.Size(); }
};

}}

