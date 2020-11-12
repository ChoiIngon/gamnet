using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Message { namespace Item {

public class MsgSvrCli_AddItem_Ntf {
	public const int MSG_ID = 130000001;
	public List<ItemData >	item_datas = new List<ItemData >();
	public MsgSvrCli_AddItem_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int);
			foreach(var item_datas_itr in item_datas) { 
				ItemData item_datas_elmt = item_datas_itr;
				nSize += ItemData_Serializer.Size(item_datas_elmt);
			}
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(item_datas.Count), 0, sizeof(int));
			foreach(var item_datas_itr in item_datas) { 
				ItemData item_datas_elmt = item_datas_itr;
				if(false == ItemData_Serializer.Store(_buf_, item_datas_elmt)) { return false; }
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int item_datas_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int item_datas_itr=0; item_datas_itr<item_datas_length; item_datas_itr++) {
				ItemData item_datas_val = new ItemData();
				if(false == ItemData_Serializer.Load(ref item_datas_val, _buf_)) { return false; }
				item_datas.Add(item_datas_val);
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_AddItem_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_AddItem_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_AddItem_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_AddItem_Ntf obj) { return obj.Size(); }
};
public class MsgSvrCli_UpdateItem_Ntf {
	public const int MSG_ID = 130000002;
	public List<ItemData >	item_datas = new List<ItemData >();
	public MsgSvrCli_UpdateItem_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += sizeof(int);
			foreach(var item_datas_itr in item_datas) { 
				ItemData item_datas_elmt = item_datas_itr;
				nSize += ItemData_Serializer.Size(item_datas_elmt);
			}
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			_buf_.Write(BitConverter.GetBytes(item_datas.Count), 0, sizeof(int));
			foreach(var item_datas_itr in item_datas) { 
				ItemData item_datas_elmt = item_datas_itr;
				if(false == ItemData_Serializer.Store(_buf_, item_datas_elmt)) { return false; }
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }
			int item_datas_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);
			_buf_.Position += sizeof(int);
			for(int item_datas_itr=0; item_datas_itr<item_datas_length; item_datas_itr++) {
				ItemData item_datas_val = new ItemData();
				if(false == ItemData_Serializer.Load(ref item_datas_val, _buf_)) { return false; }
				item_datas.Add(item_datas_val);
			}
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_UpdateItem_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_UpdateItem_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_UpdateItem_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_UpdateItem_Ntf obj) { return obj.Size(); }
};
public class MsgCliSvr_OpenPackage_Req {
	public const int MSG_ID = 130000003;
	public ulong	item_seq = 0;
	public MsgCliSvr_OpenPackage_Req() {
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
			_buf_.Write(BitConverter.GetBytes(item_seq), 0, sizeof(ulong));
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
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_OpenPackage_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_OpenPackage_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_OpenPackage_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_OpenPackage_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_OpenPackage_Ans {
	public const int MSG_ID = 130000004;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_OpenPackage_Ans() {
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
public struct MsgSvrCli_OpenPackage_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_OpenPackage_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_OpenPackage_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_OpenPackage_Ans obj) { return obj.Size(); }
};
public class MsgCliSvr_EquipItem_Req {
	public const int MSG_ID = 130000005;
	public ulong	item_seq = 0;
	public MsgCliSvr_EquipItem_Req() {
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
			_buf_.Write(BitConverter.GetBytes(item_seq), 0, sizeof(ulong));
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
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_EquipItem_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_EquipItem_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_EquipItem_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_EquipItem_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_EquipItem_Ans {
	public const int MSG_ID = 130000005;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_EquipItem_Ans() {
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
public struct MsgSvrCli_EquipItem_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_EquipItem_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_EquipItem_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_EquipItem_Ans obj) { return obj.Size(); }
};
public class MsgSvrCli_EquipItem_Ntf {
	public const int MSG_ID = 130000006;
	public ulong	item_seq = 0;
	public MsgSvrCli_EquipItem_Ntf() {
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
			_buf_.Write(BitConverter.GetBytes(item_seq), 0, sizeof(ulong));
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
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_EquipItem_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_EquipItem_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_EquipItem_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_EquipItem_Ntf obj) { return obj.Size(); }
};
public class MsgCliSvr_UnequipItem_Req {
	public const int MSG_ID = 130000007;
	public EquipItemPartType	part_type = new EquipItemPartType();
	public MsgCliSvr_UnequipItem_Req() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += EquipItemPartType_Serializer.Size(part_type);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == EquipItemPartType_Serializer.Store(_buf_, part_type)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == EquipItemPartType_Serializer.Load(ref part_type, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgCliSvr_UnequipItem_Req_Serializer {
	public static bool Store(MemoryStream _buf_, MsgCliSvr_UnequipItem_Req obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgCliSvr_UnequipItem_Req obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgCliSvr_UnequipItem_Req obj) { return obj.Size(); }
};
public class MsgSvrCli_UnequipItem_Ans {
	public const int MSG_ID = 130000007;
	public ErrorCode	error_code = new ErrorCode();
	public MsgSvrCli_UnequipItem_Ans() {
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
public struct MsgSvrCli_UnequipItem_Ans_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_UnequipItem_Ans obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_UnequipItem_Ans obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_UnequipItem_Ans obj) { return obj.Size(); }
};
public class MsgSvrCli_UnequipItem_Ntf {
	public const int MSG_ID = 130000008;
	public EquipItemPartType	part_type = new EquipItemPartType();
	public MsgSvrCli_UnequipItem_Ntf() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += EquipItemPartType_Serializer.Size(part_type);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == EquipItemPartType_Serializer.Store(_buf_, part_type)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == EquipItemPartType_Serializer.Load(ref part_type, _buf_)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
};
public struct MsgSvrCli_UnequipItem_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_UnequipItem_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_UnequipItem_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_UnequipItem_Ntf obj) { return obj.Size(); }
};

}}

