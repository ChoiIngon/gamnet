using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Message { namespace Item {

public class MsgCliSvr_OpenPackage_Req {
	public const int MSG_ID = 130000001;
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
	public const int MSG_ID = 130000002;
	public ErrorCode	error_code = new ErrorCode();
	public ItemData	item_data = new ItemData();
	public MsgSvrCli_OpenPackage_Ans() {
	}
	public virtual int Size() {
		int nSize = 0;
		try {
			nSize += ErrorCode_Serializer.Size(error_code);
			nSize += ItemData_Serializer.Size(item_data);
		} catch(System.Exception) {
			return -1;
		}
		return nSize;
	}
	public virtual bool Store(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Store(_buf_, error_code)) { return false; }
			if(false == ItemData_Serializer.Store(_buf_, item_data)) { return false; }
		} catch(System.Exception) {
			return false;
		}
		return true;
	}
	public virtual bool Load(MemoryStream _buf_) {
		try {
			if(false == ErrorCode_Serializer.Load(ref error_code, _buf_)) { return false; }
			if(false == ItemData_Serializer.Load(ref item_data, _buf_)) { return false; }
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
public class MsgSvrCli_Item_Ntf {
	public const int MSG_ID = 36;
	public List<ItemData >	item_datas = new List<ItemData >();
	public MsgSvrCli_Item_Ntf() {
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
public struct MsgSvrCli_Item_Ntf_Serializer {
	public static bool Store(MemoryStream _buf_, MsgSvrCli_Item_Ntf obj) { return obj.Store(_buf_); }
	public static bool Load(ref MsgSvrCli_Item_Ntf obj, MemoryStream _buf_) { return obj.Load(_buf_); }
	public static int Size(MsgSvrCli_Item_Ntf obj) { return obj.Size(); }
};

}}

