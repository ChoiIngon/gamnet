using UnityEngine;
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;

namespace Gamnet
{
	public class DuplicateConnectionException : Exception {
		public DuplicateConnectionException() : base() {}
		public DuplicateConnectionException(string message)	: base(message)	{}
		public DuplicateConnectionException(string message, params object[] args) : base(string.Format(message, args)) { }
		public DuplicateConnectionException(string message, Exception inner) : base(message, inner)	{}
	}
	public class Buffer	{
		public const int BUFFER_SIZE = 8192;
		public static void BlockCopy(Buffer src, Buffer dest) {
			System.Buffer.BlockCopy(src.data, src.read_index, dest.data, 0, src.Size());
			dest.read_index = 0;
			dest.write_index = src.Size();
		}

		public byte[] data = new byte[BUFFER_SIZE];
		public int read_index = 0;
		public int write_index = 0;

		public Buffer() {}
		public Buffer(byte[] src) {	Copy(src); }
		public Buffer(Buffer src) {	Copy(src); }
		public Buffer(MemoryStream src)	{ Copy(src); }

		public int Size() {	return write_index - read_index; }
		public int Available() { return BUFFER_SIZE - write_index; }

		public void Copy(byte[] src) {
			read_index = 0;
			write_index = 0;
			Append(src);
		}
		public void Copy(Buffer src) {
			read_index = 0;
			write_index = 0;
			Append(src);
		}
		public void Copy(MemoryStream src) {
			read_index = 0;
			write_index = 0;
			Append(src);
		}
		public void Append(byte[] src) {
			if (0 == src.Length) {
				return;
			}
			System.Buffer.BlockCopy(src, 0, data, write_index, src.Length);
			write_index += src.Length;
		}
		public void Append(Buffer src) {
			if (0 == src.Size()) {
				return;
			}
			System.Buffer.BlockCopy(src.data, src.read_index, data, write_index, src.Size());
			write_index += src.Size();
		}
		public void Append(System.IO.MemoryStream src) {
			if (0 == src.Position) {
				return;
			}
			System.Buffer.BlockCopy(src.GetBuffer(), 0, data, write_index, (int)src.Position);
			write_index += (int)src.Position;
		}

		public static implicit operator System.IO.MemoryStream(Gamnet.Buffer src) {
			System.IO.MemoryStream ms = new System.IO.MemoryStream();
			ms.Write(src.data, src.read_index, src.Size());
			ms.Seek(0, System.IO.SeekOrigin.Begin);
			return ms;
		}
		public static implicit operator byte[] (Gamnet.Buffer src) {
			byte[] dest = new byte[src.Size()];
			System.Buffer.BlockCopy(src.data, src.read_index, dest, 0, src.Size());
			return dest;
		}
		public static Gamnet.Buffer operator + (Gamnet.Buffer lhs, Gamnet.Buffer rhs) {
			Gamnet.Buffer buffer = new Gamnet.Buffer (lhs);
			buffer.Append(rhs);
			return buffer;
		}
	}
	public class Packet : Buffer {
		public const int OFFSET_LENGTH = 0;
		public const int OFFSET_MSGSEQ = 2;
		public const int OFFSET_MSGID = 6;
		public const int HEADER_SIZE = 10;

		private ushort 	_length;
		private uint 	_msg_seq;
		private int 	_msg_id;
		public bool 	reliable;

		public ushort length {
			set {
				byte[] buf_length = BitConverter.GetBytes(value);
				System.Buffer.BlockCopy(buf_length, 0, data, OFFSET_LENGTH, 2);
			}
			get {
				return _length;
			}
		}
		public uint msg_seq {
			set {
				byte[] buf_msg_seq = BitConverter.GetBytes(value);
				System.Buffer.BlockCopy(buf_msg_seq, 0, data, OFFSET_MSGSEQ, 4);
			}
			get {
				return _msg_seq;
			}
		}
		public int msg_id {
			set {
				byte[] buf_msg_id = BitConverter.GetBytes(value);
				System.Buffer.BlockCopy(buf_msg_id, 0, data, OFFSET_MSGID, 4);
			}
			get {
				return _msg_id;
			}
		}

		public Packet() {
			_length = 0;
			_msg_seq = 0;
			_msg_id = 0;
			reliable = false;
			write_index = HEADER_SIZE;
		}
		public Packet(Packet src) : base(src) {
			_length = src._length;
			_msg_seq = src._msg_seq;
			_msg_id = src._msg_id;
			reliable = src.reliable;
		}
	}
	public class TimeoutMonitor
	{
		private Dictionary<int, List<System.Timers.Timer>> timers = new Dictionary<int, List<System.Timers.Timer>>();
		public delegate void OnTimeout();

		public TimeoutMonitor()	{}

		public void SetTimeout(int msg_id, int timeout, System.Timers.ElapsedEventHandler timeout_callback)	{
			System.Timers.Timer timer = new System.Timers.Timer();
			timer.Interval = timeout * 1000;
			timer.AutoReset = false;
			timer.Elapsed += timeout_callback;
			timer.Start();

			if (false == timers.ContainsKey(msg_id)) {
				timers.Add(msg_id, new List<System.Timers.Timer>());
			}
			timers[msg_id].Add(timer);
		}
		public void UnsetTimeout(int msg_id) {
			if (false == timers.ContainsKey(msg_id)) {
				return;
			}
			if (0 == timers[msg_id].Count) {
				return;
			}
			timers[msg_id].RemoveAt(0);
		}
	}
	public class SyncQueue<T> {
		private object _sync_obj = new object();
        private List<T> items = new List<T>();

		public int Count() {
			lock (_sync_obj) {
				return items.Count;
			}
		}

		public void PushBack(T item) {
			lock (_sync_obj) {
				items.Add(item);
			}
		}

        public void PushFront(T item) {
            lock (_sync_obj) {
                items.Insert(0, item);
            }
        }

		public T PopFront() {
            lock (_sync_obj) {
                T item = items[0];
                items.RemoveAt(0);
                return item;
            }
		}

		public void RemoveAt(int index) {
			lock (_sync_obj) {
				items.RemoveAt(index);
			}
		}

        public T this[int index] {
            get {
				lock (_sync_obj) {
					return items [index];
				}
            }
        }

        public void Clear() {
            lock (_sync_obj) {
                items.Clear();
            }
		}

        public List<T> Copy() {
            lock (_sync_obj) {
                List<T> copy = new List<T>(items);
                return copy;
            }
        }
	};

    public class Session
    {
		public enum ConnectionState
		{
			Disconnected,
			OnConnecting,
			Connected
		}
		// length<2byte>(header length + body length) | msg_id<4byte> | body
		public const int MSG_ID_CONNECT_REQ					= 0001;
		public const int MSG_ID_CONNECT_ANS 				= 0001;
		public const int MSG_ID_RECONNECT_REQ 				= 0002;
		public const int MSG_ID_RECONNECT_ANS 				= 0002;
		public const int MSG_ID_DUPLICATE_CONNECTION_NTF 	= 0003;

		private object 		_sync_obj = new object();
        private Socket 		_socket = null;
        private IPEndPoint 	_endpoint = null;

		private ConnectionState 	_state = ConnectionState.Disconnected;
		private Gamnet.Buffer		_recv_buff = new Gamnet.Buffer();
		private TimeoutMonitor 		_timeout_monitor = null;
		private System.Timers.Timer	_connect_timer = null;

		private SyncQueue<SessionEvent> 	_event_queue = new SyncQueue<SessionEvent>();
		private SyncQueue<Gamnet.Packet>	_send_queue = new SyncQueue<Gamnet.Packet>(); // 바로 보내지 못하고 
		private int 						_send_queue_idx = 0;
		private Dictionary<int, Action<Gamnet.Buffer>> _handlers = new Dictionary<int, Action<Gamnet.Buffer>>();
		//private NetworkReachability	_networkReachability = NetworkReachability.NotReachable;
	
		private uint 	_session_key = 0;
		private string 	_session_token = "";

		private uint _msg_seq = 0;
		public uint msg_seq {
			get {
				return _msg_seq;
			}
		}

		public delegate void Delegate_OnConnect();
		public delegate void Delegate_OnReconnect();
		public delegate void Delegate_OnClose();
		public delegate void Delegate_OnError(System.Exception e);

		public Delegate_OnConnect onConnect;
		public Delegate_OnReconnect onReconnect;
		public Delegate_OnClose onClose;
		public Delegate_OnError onError;

        public abstract class SessionEvent {
            protected Session session;
			public SessionEvent(Session session) {
                this.session = session;
            }
            public abstract void Event();
        };
		public class ConnectEvent : SessionEvent {
            public ConnectEvent(Session session) : base(session) {}
            public override void Event() {
                session._state = ConnectionState.Connected;
				if (null != session.onConnect) {
					session.onConnect ();
				}
            }
        }
		public class ReconnectEvent : SessionEvent {
            public ReconnectEvent(Session session) : base(session) { }
            public override void Event() {
				session.OnReconnect ();
            }
        }
        public class ErrorEvent : SessionEvent {
            public ErrorEvent(Session session) : base(session) { }
            public System.Exception exception;
            public override void Event() {
				if (null != session.onError) {
					session.onError (exception);
				}
            }
        }
		public class CloseEvent : SessionEvent {
			public CloseEvent(Session session) : base(session) { }
			public override void Event() {
				session._state = ConnectionState.Disconnected;
				if (null != session.onClose) {
					session.onClose ();
				}
			}
		}
        public class ReceiveEvent : SessionEvent {
            public ReceiveEvent(Session session) : base(session) { }
            public Gamnet.Buffer buffer;
            public override void Event() {
                session.OnReceive(buffer);
            }
        }

		[System.Serializable] 
		class ConnectAns { 
			public int error_code = 0;
			public uint session_key = 0; 
			public string session_token = "";
		}
		[System.Serializable] 
		class ReconnectReq { 
			public uint session_key; 
			public string session_token;
		}
		[System.Serializable] 
		class ReconnectAns {
			public int error_code = 0;
			public uint msg_seq = 0;
		}

		public Session() {
			RegisterHandler (MSG_ID_CONNECT_ANS, (Gamnet.Buffer buffer) => {
				string json = System.Text.Encoding.UTF8.GetString(buffer.data, buffer.read_index, buffer.Size());
				ConnectAns ans = JsonUtility.FromJson<ConnectAns>(json);
				if(0 != ans.error_code)
				{
					Error(new System.Exception("fail to connect"));
					return;
				}
				_session_key = ans.session_key;
				_session_token = ans.session_token;

				ConnectEvent evt = new ConnectEvent(this);
				_event_queue.PushBack(evt);
			});
			RegisterHandler (MSG_ID_RECONNECT_ANS, (Gamnet.Buffer buffer) => {
				string json = System.Text.Encoding.UTF8.GetString(buffer.data, buffer.read_index, buffer.Size());
				ReconnectAns ans = JsonUtility.FromJson<ReconnectAns>(json);
				if(0 != ans.error_code)
				{
					Error(new System.Exception("fail to reconnect"));
					return;
				}
				ReconnectEvent evt = new ReconnectEvent(this);
				_event_queue.PushBack(evt);
			});
			RegisterHandler (MSG_ID_DUPLICATE_CONNECTION_NTF, (Gamnet.Buffer buffer) => {
				Error(new DuplicateConnectionException());
				Close();
			});
		}
		public void Connect(string host, int port, int timeout_sec = 60) {
            try {
				_send_queue_idx = 0;
				_send_queue.Clear();
                _timeout_monitor = new TimeoutMonitor();
				_socket = null;
				_endpoint = null;
				_connect_timer = null;
				_msg_seq = 0;
                _state = ConnectionState.OnConnecting;

                IPAddress ip = null;
                try {
                    ip = IPAddress.Parse(host);
                }
                catch (System.FormatException) {
                    IPHostEntry hostEntry = Dns.GetHostEntry(host);
                    if (hostEntry.AddressList.Length > 0) {
                        ip = hostEntry.AddressList[0];
                    }
                }
                _endpoint = new IPEndPoint(ip, port);

                Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                socket.BeginConnect(_endpoint, new AsyncCallback(Callback_Connect), socket);
				_connect_timer = new System.Timers.Timer();
				_connect_timer.Interval = timeout_sec * 1000;
				_connect_timer.AutoReset = false;
				_connect_timer.Elapsed += delegate {
					Error(new System.TimeoutException("fail to connect(" + _endpoint.ToString() + ")"));
					_socket.Close();
				};
				_connect_timer.Start();
            }
            catch (SocketException e) {
                Error(e);
            }
        }
		private void Callback_Connect(IAsyncResult result) {
			_connect_timer.Stop ();
			try	{
				_socket = (Socket)result.AsyncState;
				_socket.EndConnect(result);
				_socket.ReceiveBufferSize = Gamnet.Buffer.BUFFER_SIZE;
				_socket.SendBufferSize = Gamnet.Buffer.BUFFER_SIZE;
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 10000);
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 10000);
                //ConnectEvent evt = new ConnectEvent(this);
				//_event_queue.PushBack(evt);
				Receive();
				_state = ConnectionState.Connected;
				Gamnet.Packet packet = new Gamnet.Packet();
				packet.length = Packet.HEADER_SIZE;
				packet.msg_id = MSG_ID_CONNECT_REQ;
				packet.msg_seq = ++_msg_seq;
				SendMsg(packet);
			}
			catch (System.Exception e) {
                Error(e);
				Close();
			}
		}

        private void Reconnect() {
            if (ConnectionState.Disconnected != _state) {
                return;
            }
            
            _state = ConnectionState.OnConnecting;
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.BeginConnect(_endpoint, new AsyncCallback(Callback_Reconnect), socket);
			_connect_timer.Start();
        }
		private void Callback_Reconnect(IAsyncResult result) {
			_connect_timer.Stop ();
			try	{
				_socket = (Socket)result.AsyncState;
				_socket.EndConnect(result);
				_socket.ReceiveBufferSize = Gamnet.Buffer.BUFFER_SIZE;
				_socket.SendBufferSize = Gamnet.Buffer.BUFFER_SIZE;
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 10000);
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 10000);
                //ReconnectEvent evt = new ReconnectEvent(this);
				//_event_queue.PushBack(evt);
				Receive();

				_state = ConnectionState.Connected;

				ReconnectReq req = new ReconnectReq();
				req.session_key = _session_key;
				req.session_token = _session_token;
				byte[] data = System.Text.Encoding.UTF8.GetBytes(JsonUtility.ToJson(req));

				Gamnet.Packet packet = new Gamnet.Packet();
				packet.length = (ushort)(Packet.HEADER_SIZE + data.Length);
				packet.msg_id = MSG_ID_RECONNECT_ANS;
				packet.msg_seq = ++_msg_seq;
				packet.Append(data);

				lock (_sync_obj) {
					List<Gamnet.Packet> tmp = _send_queue.Copy ();
					_send_queue.Clear ();
					_send_queue_idx = 0;

					SendMsg(packet);
					foreach (var itr in tmp) {
						itr.read_index = 0;
						_send_queue.PushBack (itr);
					}
				}
			}
			catch (SocketException e) {
                Error(e);
				Close();
			}
		}
		public  void OnReconnect()
		{
			lock (_sync_obj) {
				
				if (null != onReconnect) {
					//List<Gamnet.Packet> tmp = _send_queue.Copy ();
					//_send_queue.Clear ();
					//_send_queue_idx = 0;

					onReconnect ();

					//foreach (var packet in tmp) {
					//	packet.read_index = 0;
					//	_send_queue.PushBack (packet);
					//}
				}
			}
		}
        
		private void Receive() {
            try {
				Gamnet.Buffer buffer = new Gamnet.Buffer();
                _socket.BeginReceive(buffer.data, 0, Gamnet.Buffer.BUFFER_SIZE, 0, new AsyncCallback(Callback_Receive), buffer);
            }
            catch (SocketException e) {
                Error(e);
                Close();
            }
        }
		private void Callback_Receive(IAsyncResult result) {
            try {
                Gamnet.Buffer buffer = (Gamnet.Buffer)result.AsyncState;
                int recvBytes = _socket.EndReceive(result);

                if (0 == recvBytes) {
                    Close();
                    return;
                }
                buffer.write_index += recvBytes;

                ReceiveEvent evt = new ReceiveEvent(this);
                evt.buffer = buffer;
                _event_queue.PushBack(evt);
                Receive();
            }
            catch (SocketException e) {
				if (ConnectionState.Disconnected == _state) {
					return;
				}
                Error(e);
                Close();
            }
		}
		public void OnReceive(Gamnet.Buffer buf) {
			_recv_buff += buf;
			while (_recv_buff.Size() >= Packet.HEADER_SIZE)
			{
				ushort packetLength = BitConverter.ToUInt16(_recv_buff.data, _recv_buff.read_index + Packet.OFFSET_LENGTH);
				if (packetLength > Gamnet.Buffer.BUFFER_SIZE) {
					throw new System.Exception(string.Format("The packet length is greater than the buffer max length."));
				}

				if (packetLength > _recv_buff.Size()) { // not enough
					return;
				}

				uint msgSEQ = BitConverter.ToUInt32(_recv_buff.data, _recv_buff.read_index + Packet.OFFSET_MSGSEQ); 
				RemoveSentPacket (msgSEQ);

				int msgID = BitConverter.ToInt32(_recv_buff.data, _recv_buff.read_index + Packet.OFFSET_MSGID);
				if (false == _handlers.ContainsKey(msgID)) {
					throw new System.Exception ("can't find registered msg(id:" + msgID + ")");
				}

				_recv_buff.read_index += Packet.HEADER_SIZE;
				_timeout_monitor.UnsetTimeout(msgID);

				Action<Gamnet.Buffer> handler = _handlers[msgID];

				try	{
					handler(_recv_buff);
				}
				catch (System.Exception e) {
					Error (e);
				}

				_recv_buff.read_index += packetLength - Packet.HEADER_SIZE;
			}
		}

        public void Error(System.Exception e) {
            ErrorEvent evt = new ErrorEvent(this);
            evt.exception = e;
            _event_queue.PushBack(evt);
        }

        public void Close() {
            if (ConnectionState.Disconnected == _state) {
                return;
            }
            try {
				_state = ConnectionState.Disconnected;
                _socket.BeginDisconnect(false, new AsyncCallback(Callback_Close), _socket);
            }
            catch (SocketException e)
            {
                Error(e);
            }
        }
		private void Callback_Close(IAsyncResult result) {
			try	{
				_socket.EndDisconnect(result);
				//_socket.Shutdown(SocketShutdown.Both);
				_socket.Close();
				CloseEvent evt = new CloseEvent(this);
				_event_queue.PushBack(evt);
			}
			catch (SocketException e) {
                Error(e);
			}
		}
        
		public TimeoutMonitor SendMsg(object msg, bool handOverRelility = false) {
			try	{
				Reconnect();

				System.IO.MemoryStream ms = new System.IO.MemoryStream();
				System.Type type = msg.GetType();
				type.GetMethod("Store").Invoke(msg, new object[] { ms });
				System.Reflection.FieldInfo fi = type.GetField("MSG_ID");

				int msgID = (int)fi.GetValue(msg);
				int dataLength = (int)(type.GetMethod("Size").Invoke(msg, null));
				ushort packetLength = (ushort)(Packet.HEADER_SIZE + dataLength);

				if (packetLength > Gamnet.Buffer.BUFFER_SIZE) {
					throw new System.Exception(string.Format("Overflow the send buffer max size : {0}", packetLength));
				}

                Gamnet.Packet packet = new Gamnet.Packet();
				packet.length = packetLength;
				packet.msg_seq = msg_seq;
				packet.msg_id = msgID;
				packet.reliable = handOverRelility;
				packet.Append(ms);

				SendMsg(packet);
			}
			catch (System.Exception e) {
                Error(e);
			}
			return _timeout_monitor;
		}
		private void SendMsg(Gamnet.Packet packet) {
			lock (_sync_obj) {
				_send_queue.PushBack(packet);
				if (1 == _send_queue.Count() - _send_queue_idx && ConnectionState.Connected == _state) {
					_socket.BeginSend(_send_queue[_send_queue_idx].data, 0, _send_queue[_send_queue_idx].Size(), 0, new AsyncCallback(Callback_SendMsg), packet);
				}
			}
		}
        private void Callback_SendMsg(IAsyncResult result) {
            try	{
                lock (_sync_obj) {
					int writedBytes = _socket.EndSend(result);
                    Gamnet.Packet packet = _send_queue[_send_queue_idx];
                    packet.read_index += writedBytes;
                    if (packet.Size() > 0) {
                        Gamnet.Packet newPacket = new Gamnet.Packet(packet);
						_socket.BeginSend(newPacket.data, 0, newPacket.Size(), 0, new AsyncCallback(Callback_SendMsg), newPacket);
                        return;
                    }

					if(true == packet.reliable) {
						_send_queue_idx++;
					}
					else {
						_send_queue.RemoveAt(_send_queue_idx);	
					}

                    if (_send_queue_idx < _send_queue.Count()) {
						_socket.BeginSend(_send_queue[_send_queue_idx].data, 0, _send_queue[_send_queue_idx].Size(), 0, new AsyncCallback(Callback_SendMsg), _send_queue[_send_queue_idx]);
						return;
                    }
                }
			}
            catch (SocketException e)
			{
				Error(e);
			}
		}

		public void RemoveSentPacket(uint msg_seq) {
			lock (_sync_obj) {
				while (0 < _send_queue.Count ()) {
					if (_send_queue [0].msg_seq > msg_seq) {
						break;
					}
					_send_queue.PopFront ();
					_send_queue_idx--;
				}
			}
		}
        public void Update() {
            while (0 < _event_queue.Count()) {
				SessionEvent evt = _event_queue.PopFront();
                evt.Event();
            }
			/*
			if (Application.internetReachability != _networkReachability) {
				_networkReachability = Application.internetReachability;
				Close ();
			}
			*/
        }
		public void RegisterHandler(int msg_id, Action<Gamnet.Buffer> handler) {
			_handlers.Add(msg_id, handler);
		}
		public void UnregisterHandler(int msg_id) {
			_handlers.Remove (msg_id);
		}
    }
}