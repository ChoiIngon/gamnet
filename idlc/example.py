import struct
class MsgCliSvr_Category_MessageName_Req :
	MSG_ID = 501
	def __init__(self) :
		self.a = 0			# uint32
		self.b = 0			# uint32

	def func(a) :
		print a
	
	def Store(self) :
		data = ''
		data += struct.pack('I', self.a)
		data += struct.pack('I', self.b)
		return data
	def Load(self, buf) :
		self.a = struct.unpack('I', buf[0:4])[0]; buf = buf[4:]
		self.b = struct.unpack('I', buf[0:4])[0]; buf = buf[4:]
		return buf
def MsgCliSvr_Category_MessageName_ReqInit() :
	return MsgCliSvr_Category_MessageName_Req()
def MsgCliSvr_Category_MessageName_ReqStore(o) :
	return o.Store()
def MsgCliSvr_Category_MessageName_ReqLoad(buf) :
	val = MsgCliSvr_Category_MessageName_Req()
	buf = val.Load(buf)
	return [val, buf]
