import struct

#from other_idl import *

class EnumTest :
	ENUM_TEST_1 = 0
	ENUM_TEST_2 = 1
def EnumTestInit() : 
	return 0
def EnumTestStore(e) :
	data = ''
	data += struct.pack('i', e)
	return data
def EnumTestLoad(buf) :
	val = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
	return [val, buf]
class EnumTest2 :
	ENUM_TEST2_1 = 100
	ENUM_TEST2_2 = 268435455
def EnumTest2Init() : 
	return 0
def EnumTest2Store(e) :
	data = ''
	data += struct.pack('i', e)
	return data
def EnumTest2Load(buf) :
	val = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
	return [val, buf]
def arr_i16_tInit() :
	return []
def arr_i16_tStore(val) :
	data = ''
	data += struct.pack('i', len(val))
	for v1 in val :
		data += struct.pack('h', v1)
	return data
def arr_i16_tLoad(buf) :
	elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
	val = []
	for i in range(elmt_count) :
		v1 = struct.unpack('h', buf[0:2])[0]; buf = buf[2:]
		val.append(v1)
	return [val, buf]
def complex_arr_i16_tInit() :
	return []
def complex_arr_i16_tStore(val) :
	data = ''
	data += struct.pack('i', len(val))
	for v1 in val :
		data += arr_i16_tStore(v1)
	return data
def complex_arr_i16_tLoad(buf) :
	elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
	val = []
	for i in range(elmt_count) :
		[v1, buf] = arr_i16_tLoad(buf)
		val.append(v1)
	return [val, buf]
def lst_string_tInit() :
	return []
def lst_string_tStore(val) :
	data = ''
	data += struct.pack('i', len(val))
	for v1 in val :
		data += struct.pack('i', len(v1))
		fmt = str(len(v1)) + 's'
		data += struct.pack(fmt, v1)
	return data
def lst_string_tLoad(buf) :
	elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
	val = []
	for i in range(elmt_count) :
		str_length = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		v1 = buf[0:str_length]; buf = buf[str_length:]
		val.append(v1)
	return [val, buf]
def complex_lst_string_tInit() :
	return []
def complex_lst_string_tStore(val) :
	data = ''
	data += struct.pack('i', len(val))
	for v1 in val :
		data += lst_string_tStore(v1)
	return data
def complex_lst_string_tLoad(buf) :
	elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
	val = []
	for i in range(elmt_count) :
		[v1, buf] = lst_string_tLoad(buf)
		val.append(v1)
	return [val, buf]
def map_i32_string_tInit() :
	return {}
def map_i32_string_tStore(val) :
	data = ''
	data += struct.pack('i', len(val))
	for k1, v1 in val.iteritems() : 
		data += struct.pack('i', k1)
		data += struct.pack('i', len(v1))
		fmt = str(len(v1)) + 's'
		data += struct.pack(fmt, v1)
	return data
def map_i32_string_tLoad(buf) :
	elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
	for i  in range(elmt_count) :
		k1 = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		str_length = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		v1 = buf[0:str_length]; buf = buf[str_length:]
		val[k1] = v1
	return [val, buf]
def complex_map_i32_string_tInit() :
	return {}
def complex_map_i32_string_tStore(val) :
	data = ''
	data += struct.pack('i', len(val))
	for k1, v1 in val.iteritems() : 
		data += struct.pack('i', len(k1))
		fmt = str(len(k1)) + 's'
		data += struct.pack(fmt, k1)
		data += map_i32_string_tStore(v1)
	return data
def complex_map_i32_string_tLoad(buf) :
	elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
	for i  in range(elmt_count) :
		str_length = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		k1 = buf[0:str_length]; buf = buf[str_length:]
		[v1, buf] = map_i32_string_tLoad(buf)
		val[k1] = v1
	return [val, buf]
class DefaultData :
	def __init__(self) :
		self.byte_ = '\0'			# byte
		self.ubyte_ = '\0'			# ubyte
		self.boolean_ = False			# boolean
		self.i16_ = 0			# int16
		self.i32_ = 0			# int32
		self.i64_ = 0			# int64
		self.ui16_ = 0			# uint16
		self.ui32_ = 0			# uint32
		self.ui64_ = 0			# uint64
		self.double_ = 0.0			# double
		self.float_ = 0.0			# float
		self.string_ = ''			# string
	def Store(self) :
		data = ''
		data += struct.pack('c', self.byte_)
		data += struct.pack('c', self.ubyte_)
		data += struct.pack('b', self.boolean_)
		data += struct.pack('h', self.i16_)
		data += struct.pack('i', self.i32_)
		data += struct.pack('q', self.i64_)
		data += struct.pack('H', self.ui16_)
		data += struct.pack('I', self.ui32_)
		data += struct.pack('Q', self.ui64_)
		data += struct.pack('d', self.double_)
		data += struct.pack('f', self.float_)
		data += struct.pack('i', len(self.string_))
		fmt = str(len(self.string_)) + 's'
		data += struct.pack(fmt, self.string_)
		return data
	def Load(self, buf) :
		self.byte_ = struct.unpack('c', buf[0:1])[0]; buf = buf[1:]
		self.ubyte_ = struct.unpack('c', buf[0:1])[0]; buf = buf[1:]
		self.boolean_ = struct.unpack('b', buf[0:1])[0]; buf = buf[1:]
		self.i16_ = struct.unpack('h', buf[0:2])[0]; buf = buf[2:]
		self.i32_ = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		self.i64_ = struct.unpack('q', buf[0:8])[0]; buf = buf[8:]
		self.ui16_ = struct.unpack('H', buf[0:2])[0]; buf = buf[2:]
		self.ui32_ = struct.unpack('I', buf[0:4])[0]; buf = buf[4:]
		self.ui64_ = struct.unpack('Q', buf[0:8])[0]; buf = buf[8:]
		self.double_ = struct.unpack('d', buf[0:8])[0]; buf = buf[8:]
		self.float_ = struct.unpack('f', buf[0:4])[0]; buf = buf[4:]
		str_length = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		self.string_ = buf[0:str_length]; buf = buf[str_length:]
		return buf
def DefaultDataInit() :
	return DefaultData()
def DefaultDataStore(o) :
	return o.Store()
def DefaultDataLoad(buf) :
	val = DefaultData()
	buf = val.Load(buf)
	return [val, buf]
class ContainerData :
	def __init__(self) :
		self.map_ = {}			# map
		self.map_i32_string_ = map_i32_string_tInit()			# map_i32_string_t
		self.array_ = []			# array
		self.static_array_ = [''] * 256			# static_array
		self.list_ = []			# list
	def Store(self) :
		data = ''
		data += struct.pack('i', len(self.map_))
		for k2, v2 in self.map_.iteritems() : 
			data += struct.pack('i', k2)
			data += struct.pack('i', len(v2))
			fmt = str(len(v2)) + 's'
			data += struct.pack(fmt, v2)
		data += map_i32_string_tStore(self.map_i32_string_)
		data += struct.pack('i', len(self.array_))
		for v2 in self.array_ :
			data += struct.pack('i', len(v2))
			fmt = str(len(v2)) + 's'
			data += struct.pack(fmt, v2)
		self.static_array_ = list(self.static_array_) + [''] * (256 - len(self.static_array_))
		for k2 in range(0, 256) :
			data += struct.pack('i', len(self.static_array_[k2]))
			fmt = str(len(self.static_array_[k2])) + 's'
			data += struct.pack(fmt, self.static_array_[k2])
		data += struct.pack('i', len(self.list_))
		for v2 in self.list_ :
			data += struct.pack('i', v2)
		return data
	def Load(self, buf) :
		elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		for i  in range(elmt_count) :
			k2 = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
			str_length = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
			v2 = buf[0:str_length]; buf = buf[str_length:]
			self.map_[k2] = v2
		[self.map_i32_string_, buf] = map_i32_string_tLoad(buf)
		elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		array_ = []
		for i in range(elmt_count) :
			str_length = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
			v2 = buf[0:str_length]; buf = buf[str_length:]
			self.array_.append(v2)
		self.static_array_ = []
		for i in range(0, 256) :
			str_length = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
			v2 = buf[0:str_length]; buf = buf[str_length:]
			self.static_array_.append(v2)
		elmt_count = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		list_ = []
		for i in range(elmt_count) :
			v2 = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
			self.list_.append(v2)
		return buf
def ContainerDataInit() :
	return ContainerData()
def ContainerDataStore(o) :
	return o.Store()
def ContainerDataLoad(buf) :
	val = ContainerData()
	buf = val.Load(buf)
	return [val, buf]
class DerivedData (DefaultData) :
	def __init__(self) :
		DefaultData.__init__(self)
		self.containerData_ = ContainerDataInit()			# ContainerData
	def Store(self) :
		data = ''
		data += DefaultData.Store(self)
		data += ContainerDataStore(self.containerData_)
		return data
	def Load(self, buf) :
		buf = DefaultData.Load(self, buf)
		[self.containerData_, buf] = ContainerDataLoad(buf)
		return buf
def DerivedDataInit() :
	return DerivedData()
def DerivedDataStore(o) :
	return o.Store()
def DerivedDataLoad(buf) :
	val = DerivedData()
	buf = val.Load(buf)
	return [val, buf]
class MsgCliSvr_Category_MessageName_Req :
	MSG_ID = 501
	def __init__(self) :
		pass
	def Store(self) :
		data = ''
		return data
	def Load(self, buf) :
		return buf
def MsgCliSvr_Category_MessageName_ReqInit() :
	return MsgCliSvr_Category_MessageName_Req()
def MsgCliSvr_Category_MessageName_ReqStore(o) :
	return o.Store()
def MsgCliSvr_Category_MessageName_ReqLoad(buf) :
	val = MsgCliSvr_Category_MessageName_Req()
	buf = val.Load(buf)
	return [val, buf]
class MsgSvrCli_Category_MessageName_Ans :
	MSG_ID = 502
	def __init__(self) :
		self.nErrorCode = 0			# int32
		self.derivedData = DerivedDataInit()			# DerivedData
	def Store(self) :
		data = ''
		data += struct.pack('i', self.nErrorCode)
		data += DerivedDataStore(self.derivedData)
		return data
	def Load(self, buf) :
		self.nErrorCode = struct.unpack('i', buf[0:4])[0]; buf = buf[4:]
		[self.derivedData, buf] = DerivedDataLoad(buf)
		return buf
def MsgSvrCli_Category_MessageName_AnsInit() :
	return MsgSvrCli_Category_MessageName_Ans()
def MsgSvrCli_Category_MessageName_AnsStore(o) :
	return o.Store()
def MsgSvrCli_Category_MessageName_AnsLoad(buf) :
	val = MsgSvrCli_Category_MessageName_Ans()
	buf = val.Load(buf)
	return [val, buf]
