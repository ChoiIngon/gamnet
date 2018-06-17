﻿using UnityEngine;
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;

namespace Gamnet
{
	public class Packet : Buffer {
		public const int OFFSET_LENGTH = 0;
		public const int OFFSET_MSGSEQ = 2;
		public const int OFFSET_MSGID = 6;
        public const int OFFSET_RELIABLE = 10;
        public const int OFFSET_RESERVED = 11;
		public const int HEADER_SIZE = 12;

		private ushort 	_length;
		private uint 	_msg_seq;
		private uint 	_msg_id;
		private bool 	_reliable;

		public ushort length {
			set {
				byte[] buf_length = BitConverter.GetBytes(value);
				System.Buffer.BlockCopy(buf_length, 0, data, OFFSET_LENGTH, 2);
                _length = value;
			}
			get {
				return _length;
			}
		}
		public uint msg_seq {
			set {
				byte[] buf_msg_seq = BitConverter.GetBytes(value);
				System.Buffer.BlockCopy(buf_msg_seq, 0, data, OFFSET_MSGSEQ, 4);
                _msg_seq = value;
			}
			get {
				return _msg_seq;
			}
		}
		public uint msg_id {
			set {
				byte[] buf_msg_id = BitConverter.GetBytes(value);
				System.Buffer.BlockCopy(buf_msg_id, 0, data, OFFSET_MSGID, 4);
                _msg_id = value;
			}
			get {
				return _msg_id;
			}
		}
        public bool reliable {
            set
            {
                byte[] buf_reliable = BitConverter.GetBytes(value);
                System.Buffer.BlockCopy(buf_reliable, 0, data, OFFSET_RELIABLE, 1);
                _reliable = value;
            }
            get
            {
                return _reliable;
            }
        }

		public Packet() {
			_length = 0;
			_msg_seq = 0;
			_msg_id = 0;
			_reliable = false;
			write_index = HEADER_SIZE;
		}
		public Packet(Packet src) : base(src) {
			_length = src._length;
			_msg_seq = src._msg_seq;
			_msg_id = src._msg_id;
			_reliable = src._reliable;
		}
	}
	public class TimeoutMonitor
	{
		private Dictionary<uint, List<System.Timers.Timer>> timers = new Dictionary<uint, List<System.Timers.Timer>>();
		public delegate void OnTimeout();

		public TimeoutMonitor()	{}

		public void SetTimeout(uint msg_id, int timeout, OnTimeout timeout_callback)	{
			System.Timers.Timer timer = new System.Timers.Timer();
			timer.Interval = timeout * 1000;
			timer.AutoReset = false;
			timer.Elapsed += delegate {
				timeout_callback();
			};
			timer.Start();

			if (false == timers.ContainsKey(msg_id)) {
				timers.Add(msg_id, new List<System.Timers.Timer>());
			}
			timers[msg_id].Add(timer);
		}
		public void UnsetTimeout(uint msg_id) {
			if (false == timers.ContainsKey(msg_id)) {
				return;
			}
			if (0 == timers[msg_id].Count) {
				return;
			}

            timers[msg_id][0].Stop();
            timers[msg_id].RemoveAt(0);
		}
	}
	
    public class Session
    {
		public enum ConnectionState
		{
			Disconnected,
			OnConnecting,
			Connected
		}

        public enum DisconnectState
        {
            Invalid,
            Pause,
            Handover,
            Closed
        }

		public abstract class IMsgHandler {
			public abstract void OnRecvMsg (System.IO.MemoryStream ms);
		}

		public class MsgHandler<T> : IMsgHandler where T : new() {
			private Session session;
			public MsgHandler(Session session) {
				this.session = session;
			}
			public Action<T> onReceive;
			public override void OnRecvMsg(System.IO.MemoryStream ms)
			{
				if (null == onReceive) {
					return;
				}
				T msg = new T ();
				System.Type type = msg.GetType();
				if (false == (bool)type.GetMethod ("Load").Invoke (msg, new object[] { ms })) {
					Debug.LogError(ErrorCode.MessageFormatError.ToString() + " message load fail(msg:" + type.Name + ")");
					session.Error(new Gamnet.Exception(ErrorCode.MessageFormatError, " message load fail(msg:" + type.Name + ")"));
					return;
				}
				onReceive (msg);
			}
		}
		// length<2byte>(header length + body length) | msg_id<4byte> | body

		public int 			heartbeat_time = 60000;
		private int 		_connect_timeout = 60000;
		private object 		_sync_obj = new object();
        private Socket 		_socket = null;
        private IPEndPoint 	_endpoint = null;
        private bool        _connect_complete = false;
		private ConnectionState 	_state = ConnectionState.Disconnected;
        public ConnectionState      state { get { return _state; } }
        private DisconnectState     _disconnectState = DisconnectState.Invalid;
		private Gamnet.Buffer		_recv_buff = new Gamnet.Buffer();
		private TimeoutMonitor 		_timeout_monitor = null;
		private System.Timers.Timer	_timer = null;

		private List<SessionEvent> 	_event_queue = new List<SessionEvent>();
		private List<Gamnet.Packet>	_send_queue = new List<Gamnet.Packet>(); // 바로 보내지 못하고 
		private int 				_send_queue_idx = 0;

		private Dictionary<uint, IMsgHandler> _handlers = new Dictionary<uint, IMsgHandler>();

		private NetworkReachability	_networkReachability = NetworkReachability.NotReachable;
	
		private uint 	_session_key = 0;
		private string 	_session_token = "";

		private uint _send_seq = 0;
        private uint _recv_seq = 0;
		
		public Action onConnect;
		public Action onReconnect;
        public Action onResume;
		public Action onClose;
		public Action<Gamnet.Exception> onError;

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
				if (null != session.onConnect) {
					session.onConnect ();
				}
            }
        }
		public class ReconnectEvent : SessionEvent {
            public ReconnectEvent(Session session) : base(session) { }
            public override void Event() {
				if (null != session.onReconnect) {
					session.onReconnect ();
				}
            }
        }
        public class ResumeEvent : SessionEvent {
            public ResumeEvent(Session session) : base(session) { }
            public override void Event() {
                if (null != session.onResume) {
                    session.onResume();
                }
            }
        }
        public class ErrorEvent : SessionEvent {
            public ErrorEvent(Session session) : base(session) { }
			public Gamnet.Exception exception;
            public override void Event() {
				if (null != session.onError) {
					session.onError (exception);
				}
            }
        }
		public class CloseEvent : SessionEvent {
			public CloseEvent(Session session) : base(session) { }
			public override void Event() {
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

		public Session() {
			_handlers.Add(MsgID_SvrCli_Connect_Ans, new ConnectHandler (this));
			_handlers.Add(MsgID_SvrCli_Reconnect_Ans, new ReconnectHandler (this));
			_handlers.Add(MsgID_SvrCli_HeartBeat_Ans, new HeartBeatHandler (this));
			_handlers.Add(MsgID_SvrCli_Kickout_Ntf, new KickoutHandler (this));
            _handlers.Add(MsgID_SvrCli_Close_Ans, new CloseHandler(this));
		}
		public void Connect(string host, int port, int timeout_sec = 60) {
            try {
				lock(_sync_obj) {
					_send_queue_idx = 0;
					_send_queue.Clear();
				}

                _networkReachability = Application.internetReachability;
                _timeout_monitor = new TimeoutMonitor();
				_socket = null;
				_endpoint = null;
				_timer = null;
				_send_seq = 0;
                _recv_seq = 0;
                _state = ConnectionState.OnConnecting;
				_connect_timeout = timeout_sec * 1000;

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

				Socket socket = new Socket(_endpoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                socket.BeginConnect(_endpoint, new AsyncCallback(Callback_Connect), socket);
				SetConnectTimeout();
            }
            catch (SocketException e) {
				Debug.LogError ("[Session.Connect] host:" + host + ", port:" + port + ", timeout_sec:" + timeout_sec + ", exception:" + e.ToString ());
				Error(new Gamnet.Exception(ErrorCode.ConnectFailError, e.ToString()));
            }
        }
		private void Callback_Connect(IAsyncResult result) {
            try	{
				_socket = (Socket)result.AsyncState;
				_socket.EndConnect(result);
				_socket.ReceiveBufferSize = Gamnet.Buffer.BUFFER_SIZE;
				_socket.SendBufferSize = Gamnet.Buffer.BUFFER_SIZE;
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 10000);
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 10000);
				_timer.Stop ();

				Receive();

				_state = ConnectionState.Connected;
				_disconnectState = DisconnectState.Invalid;

				Send_Connect_Req();

                _timer = new System.Timers.Timer();
				_timer.Interval = heartbeat_time;
                _timer.AutoReset = true;
                _timer.Elapsed += delegate { 
					Send_HeartBeat_Req(); 
				};
                _timer.Start();
            }
			catch (System.Exception e) {
				Debug.LogError ("[Session.Callback_Connect] exception:" + e.ToString ());
				Error(new Gamnet.Exception(ErrorCode.ConnectFailError, e.ToString()));
				Disconnect();
			}
		}

        public void Resume()
        {
            if (false == _connect_complete)
            {
                return;
            }
			Debug.Log ("[Session.Resume]");
            Reconnect();
        }

        public void Pause()
        {
			Debug.Log ("[Session.Pause]");
            Disconnect();
            _disconnectState = DisconnectState.Pause;
        }

        private void Reconnect() {
			if (null == _endpoint)
			{
				Debug.LogError("[Session.Reconnect] invalid destination address");
				return;
			}
            if (ConnectionState.Disconnected != _state) {
                return;
            }
            if (DisconnectState.Closed == _disconnectState) {
				Debug.LogError ("[Session.Reconnect] closed session should not try to reconnect");
                return;
            }
            _state = ConnectionState.OnConnecting;
            Socket socket = new Socket(_endpoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            socket.BeginConnect(_endpoint, new AsyncCallback(Callback_Reconnect), socket);
			SetConnectTimeout();
        }
		private void Callback_Reconnect(IAsyncResult result) {
			try	{
				_socket = (Socket)result.AsyncState;
				_socket.EndConnect(result);
				_socket.ReceiveBufferSize = Gamnet.Buffer.BUFFER_SIZE;
				_socket.SendBufferSize = Gamnet.Buffer.BUFFER_SIZE;
                _timer.Stop ();
                
                Receive();
                
                _state = ConnectionState.Connected;

				lock (_sync_obj) {
					Send_Reconnect_Req();
                    Send_HeartBeat_Req();
                }
                _timer = new System.Timers.Timer();
				_timer.Interval = heartbeat_time;
                _timer.AutoReset = true;
                _timer.Elapsed += delegate { 
					Send_HeartBeat_Req(); 
				};
                _timer.Start();
            }
			catch (SocketException e) {
				Debug.LogError ("[Session.Callback_Reconnect] exception:" + e.ToString ());
				Error(new Gamnet.Exception(ErrorCode.ConnectFailError, e.ToString()));
				Disconnect();
			}
		}

		private void Receive() {
            try {
				Gamnet.Buffer buffer = new Gamnet.Buffer();
                _socket.BeginReceive(buffer.data, 0, Gamnet.Buffer.BUFFER_SIZE, 0, new AsyncCallback(Callback_Receive), buffer);
            }
            catch (SocketException e) {
				Debug.LogError ("[Session.Receive] exception:" + e.ToString ());
				Error(new Gamnet.Exception(ErrorCode.UndefinedError, e.ToString()));
				Disconnect();
			}
        }
		private void Callback_Receive(IAsyncResult result) {
            try {
                Gamnet.Buffer buffer = (Gamnet.Buffer)result.AsyncState;
                int recvBytes = _socket.EndReceive(result);
                if (0 == recvBytes) {
                    Debug.Log("socket read '0' byte. close socket");
                    Disconnect();
                    return;
                }
                buffer.write_index += recvBytes;

                ReceiveEvent evt = new ReceiveEvent(this);
                evt.buffer = buffer;
				lock(_sync_obj) {
                	_event_queue.Add(evt);
				}
				Receive();
            }
            catch (SocketException e) {
                Debug.LogError("[Session.Callback_Receive] session_state:" + _state + ", exception:" + e.ToString());
                if (ConnectionState.Disconnected == _state) {
					return;
				}
				Error(new Gamnet.Exception(ErrorCode.UndefinedError, e.ToString()));
                Disconnect();
            }
		}
		private void OnReceive(Gamnet.Buffer buf) {
			_recv_buff += buf;
			while (_recv_buff.Size() >= Packet.HEADER_SIZE)
			{
				ushort packetLength = BitConverter.ToUInt16(_recv_buff.data, _recv_buff.read_index + Packet.OFFSET_LENGTH);
                uint msgID = BitConverter.ToUInt32(_recv_buff.data, _recv_buff.read_index + Packet.OFFSET_MSGID);
                uint msgSEQ = BitConverter.ToUInt32(_recv_buff.data, _recv_buff.read_index + Packet.OFFSET_MSGSEQ);
                bool reliable = BitConverter.ToBoolean(_recv_buff.data, _recv_buff.read_index + Packet.OFFSET_RELIABLE);

                if (packetLength > Gamnet.Buffer.BUFFER_SIZE) {
					Debug.LogError ("[Session.OnReceive] The packet length is greater than the buffer max length.");
					Error(new Gamnet.Exception(ErrorCode.UndefinedError, "The packet length is greater than the buffer max length."));
                    return;
				}

				if (packetLength > _recv_buff.Size()) { // not enough
					return;
				}

                _recv_buff.read_index += Packet.HEADER_SIZE;
                
                _timeout_monitor.UnsetTimeout(msgID);

                try
                {
                    if (false == _handlers.ContainsKey(msgID))
                    {
                        Debug.LogWarning("[Session.OnReceive] can't find registered msg(id:" + msgID + ")");
                        throw new Gamnet.Exception(ErrorCode.UnhandledMsgError, "can't find registered msg(id:" + msgID + ")");
                    }

					System.IO.MemoryStream ms = new System.IO.MemoryStream();
					ms.Write(_recv_buff.data, _recv_buff.read_index, packetLength - Packet.HEADER_SIZE);
					ms.Seek(0, System.IO.SeekOrigin.Begin);
                    IMsgHandler handler = _handlers[msgID];
					handler.OnRecvMsg(ms);
					if (msgID > MsgID_Max && true == reliable)
                    {
                        if (msgSEQ > _recv_seq)
                        {
                            _recv_seq = msgSEQ;
                            Send_ReliableAck_Ntf(_recv_seq);
                        }
                    }
                }
                catch (Gamnet.Exception e)
                {
                    Debug.LogError("[Session.OnReceive] " + e.ToString());
                    Error(e);
                }
                catch (System.Exception e)
                {
                    Debug.LogError("[Session.OnReceive] " + e.ToString());
                    Error(new Gamnet.Exception(ErrorCode.UndefinedError, e.ToString()));
                }
               
				_recv_buff.read_index += packetLength - Packet.HEADER_SIZE;
			}
		}

		public void Error(Gamnet.Exception e) {
            ErrorEvent evt = new ErrorEvent(this);
            evt.exception = e;
			lock (_sync_obj) {
				_event_queue.Add (evt);
			}
        }

        public void Close() {
			Debug.Log("[Session.Close]");
            Send_Close_Req();
        }

        private void Disconnect()
        {
            if (ConnectionState.Disconnected == _state)
            {
                return;
            }
			Debug.Log("[Session.Disconnect] session state:" + _state + ", disconnect state:" + _disconnectState);
            try
            {
                _state = ConnectionState.Disconnected;
                _timer.Stop();

                if (null == _socket)
                {
                    return;
                }
                //_socket.Shutdown(SocketShutdown.Send);
                //_socket.BeginDisconnect(false, new AsyncCallback(Callback_Disconnect), _socket);
                _socket.Close();
                CloseEvent evt = new CloseEvent(this);
                lock (_sync_obj)
                {
                    _event_queue.Add(evt);
                }
            }
            catch (SocketException e)
            {
                Debug.LogError("[Session.Disconnect] exception:" + e.ToString());
                Error(new Gamnet.Exception(ErrorCode.UndefinedError, e.ToString()));
            }
            catch (ObjectDisposedException e)
            {
                Debug.LogError("[Session.Disconnect] exception:" + e.ToString());
            }
        }
        /*
		private void Callback_Disconnect(IAsyncResult result) {
			try	{
				_socket.EndDisconnect(result);
				
				CloseEvent evt = new CloseEvent(this);
				lock(_sync_obj)	{
					_event_queue.Add(evt);
				}
			}
			catch (SocketException e) {
                Debug.Log ("[Session.Callback_Disconnect] session_state:" + _state + ", exception:" + e.ToString ());
			}
		}
        */
		public TimeoutMonitor SendMsg(object msg, bool handOverRelility = false) {
			if (null == _endpoint)
			{
				Debug.LogError("[Session.Reconnect] invalid destination address");
				return null;
			}
            if (true == handOverRelility)
            {
                if (Application.internetReachability != _networkReachability)
                {
                    _networkReachability = Application.internetReachability;
                    Disconnect();
                    _disconnectState = DisconnectState.Handover;
                }
            }
            try
            {
				Reconnect();

				System.IO.MemoryStream ms = new System.IO.MemoryStream();
				System.Type type = msg.GetType();
				type.GetMethod("Store").Invoke(msg, new object[] { ms });
				System.Reflection.FieldInfo fi = type.GetField("MSG_ID");

				uint msgID = (uint)(int)fi.GetValue(msg);
				int dataLength = (int)(type.GetMethod("Size").Invoke(msg, null));
				ushort packetLength = (ushort)(Packet.HEADER_SIZE + dataLength);

				if (packetLength > Gamnet.Buffer.BUFFER_SIZE) {
					throw new System.Exception(string.Format("Overflow the send buffer max size : {0}", packetLength));
				}

                Gamnet.Packet packet = new Gamnet.Packet();
				packet.length = packetLength;
				packet.msg_seq = ++_send_seq;
				packet.msg_id = msgID;
				packet.reliable = handOverRelility;
				packet.Append(ms);

				SendMsg(packet);
				if (0 == _send_seq % 30 && ConnectionState.Connected == _state)
				{
					Send_HeartBeat_Req ();
				}
			}
			catch (System.Exception e) {
				Debug.LogError ("[Session.SendMsg] exception:" + e.ToString ());
				Error(new Gamnet.Exception(ErrorCode.SendMsgFailError, e.ToString()));
				return null;
			}
			return _timeout_monitor;
		}
		private void SendMsg(Gamnet.Packet packet) {
            lock (_sync_obj) {
				_send_queue.Add(packet);
				if (ConnectionState.Connected == _state && 1 == _send_queue.Count - _send_queue_idx) {
                    //Debug.Log("begin send 1(msg_seq:" + _send_queue[_send_queue_idx].msg_seq + ", msg_id:" + _send_queue[_send_queue_idx].msg_id + ")");
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
                    if (0 < packet.Size()) {
						Debug.LogWarning("[Session.Callback_SendMsg] IO pendding, cause of packet size(remain bytes:" + packet.Size() + ")");
                        Gamnet.Packet newPacket = new Gamnet.Packet(packet);
						_socket.BeginSend(newPacket.data, 0, newPacket.Size(), 0, new AsyncCallback(Callback_SendMsg), newPacket);
                        return;
                    }
                    //Debug.Log("end send(msg_seq:" + _send_queue[_send_queue_idx].msg_seq + ", msg_id:" + _send_queue[_send_queue_idx].msg_id + ", length:" + _send_queue[_send_queue_idx].length + ")");
                    if (true == packet.reliable) {
						_send_queue_idx++;
					}
					else {
						_send_queue.RemoveAt(_send_queue_idx);	
					}

                    if (_send_queue_idx < _send_queue.Count) {
                        //Debug.Log("begin send 2(msg_seq:" + _send_queue[_send_queue_idx].msg_seq + ", msg_id:" + _send_queue[_send_queue_idx].msg_id + ", length:" + _send_queue[_send_queue_idx].length + ")");
                        _socket.BeginSend(_send_queue[_send_queue_idx].data, 0, _send_queue[_send_queue_idx].Size(), 0, new AsyncCallback(Callback_SendMsg), _send_queue[_send_queue_idx]);
						return;
                    }
                }
			}
            catch (SocketException e)
			{
				Debug.LogError ("[Session.Callback_SendMsg] exception:" + e.ToString ());
				Error(new Gamnet.Exception(ErrorCode.SendMsgFailError, e.ToString()));
			}
		}

		void RemoveSentPacket(uint msg_seq) {
			lock (_sync_obj) {
				while (0 < _send_queue.Count) {
					if (_send_queue [0].msg_seq > msg_seq) {
						break;
					}
                    if (true == _send_queue[0].reliable)
                    {
                        _send_queue_idx--;
                    }
                    _send_queue.RemoveAt(0);
                }
                
            }
		}
		void SetConnectTimeout() {
			_timer = new System.Timers.Timer();
			_timer.Interval = _connect_timeout;
			_timer.AutoReset = false;
			_timer.Elapsed += delegate {
				Error(new Gamnet.Exception(ErrorCode.ConnectTimeoutError, "connect timeout. exception(" + _endpoint.ToString() + ")"));
				Disconnect();
			};
			_timer.Start();
		}
        public void Update() {
			lock (_sync_obj) {
				while (0 < _event_queue.Count) {
					SessionEvent evt = _event_queue[0];
					_event_queue.RemoveAt (0);
					evt.Event ();
				}
			}
        }

		public void RegisterHandler<T> (Action<T> handler) where T : new()
		{
			System.Type msg_type = typeof(T);
			System.Reflection.FieldInfo fieldInfo = msg_type.GetField ("MSG_ID");
			int msg_id = (int)fieldInfo.GetValue (null);
			MsgHandler<T> msg_handler = null;
			if (true == _handlers.ContainsKey ((uint)msg_id)) {
				msg_handler = (MsgHandler<T>)_handlers [(uint)msg_id];
			} else {
				msg_handler = new MsgHandler<T> (this);
				_handlers.Add ((uint)msg_id, msg_handler);
			}
			msg_handler.onReceive += handler;
		}
		public void UnregisterHandler<T>(Action<T> handler = null) where T : new()
        {
			System.Type msg_type = typeof(T);
			System.Reflection.FieldInfo fieldInfo = msg_type.GetField ("MSG_ID");
			int msg_id = (int)fieldInfo.GetValue (null);

			if (false == _handlers.ContainsKey ((uint)msg_id)) {
				return;
			}
			MsgHandler<T> msg_handler = (MsgHandler<T>)_handlers [(uint)msg_id];
			if (null != handler) {
				msg_handler.onReceive -= handler;
			} else {
				msg_handler.onReceive = null;
			}

			if (null == msg_handler.onReceive) {
				_handlers.Remove ((uint)msg_id);
			}
        }
			
		const uint MsgID_CliSvr_Connect_Req = 0001;
        const uint MsgID_SvrCli_Connect_Ans = 0001;
        const uint MsgID_CliSvr_Reconnect_Req = 0002;
        const uint MsgID_SvrCli_Reconnect_Ans = 0002;
        const uint MsgID_CliSvr_HeartBeat_Req = 0003;
        const uint MsgID_SvrCli_HeartBeat_Ans = 0003;
        const uint MsgID_CliSvr_ReliableAck_Ntf	= 4;
		const uint MsgID_SvrCli_ReliableAck_Ntf	= 4;
		const uint MsgID_SvrCli_Kickout_Ntf		= 5;
		const uint MsgID_CliSvr_Close_Req		= 6;
        const uint MsgID_SvrCli_Close_Ans       = 6;
		const uint MsgID_Max						= 7;

        void Send_Connect_Req()
		{
			Gamnet.Packet packet = new Gamnet.Packet();
			packet.length = Packet.HEADER_SIZE;
            packet.msg_seq = 0; // ++_send_seq;
			packet.msg_id = MsgID_CliSvr_Connect_Req;
			packet.reliable = false;
			SendMsg(packet);
		}

        [System.Serializable]
        class Msg_Connect_Ans
        {
            public int error_code = 0;
            public uint session_key = 0;
            public string session_token = "";
        }
        public class ConnectHandler : IMsgHandler {
            private Session session;

			public ConnectHandler(Session session) {
				this.session = session;
			}

			public override void OnRecvMsg (System.IO.MemoryStream buffer)
			{
				string json = System.Text.Encoding.UTF8.GetString(buffer.GetBuffer(), (int)buffer.Position, (int)buffer.Length);
				Debug.Log (LogHeader.Function + "position:" + (int)buffer.Position + ", length:" + (int)(buffer.Length - buffer.Position) + ", json:" + json);
				Msg_Connect_Ans ans = JsonUtility.FromJson<Msg_Connect_Ans>(json);
				if(0 != ans.error_code)
				{
					Debug.LogError ("[ConnectHandler.OnRecvMsgrecv] connect fail(error_code:" + ans.error_code +")");
					session.Error(new Gamnet.Exception(ErrorCode.ConnectFailError, "connect fail(error_code:" + ans.error_code +")"));
					return;
				}
				Debug.Log("[ConnectHandler.OnRecvMsgrecv] connect ans(session_key:" + ans.session_key + ", session_token:" + ans.session_token + ")");
				session._session_key = ans.session_key;
				session._session_token = ans.session_token;
                session._connect_complete = true;

				ConnectEvent evt = new ConnectEvent(session);
				session._event_queue.Add(evt); // already locked
			}
		}
        		
		[System.Serializable] 
		class Msg_Reconnect_Req { 
			public uint session_key = 0; 
			public string session_token = "";
		}
		void Send_Reconnect_Req()
		{
			Msg_Reconnect_Req req = new Msg_Reconnect_Req();
			req.session_key = _session_key;
			req.session_token = _session_token;
            string json = JsonUtility.ToJson(req);
            byte[] data = System.Text.Encoding.UTF8.GetBytes(json);

            Gamnet.Packet packet = new Gamnet.Packet();
            packet.length = (ushort)(Packet.HEADER_SIZE + data.Length);
            packet.msg_seq = 0;// ++_send_seq;
            packet.msg_id = MsgID_CliSvr_Reconnect_Req;
            packet.reliable = false;
            packet.Append(data);

            List<Gamnet.Packet> tmp = new List<Gamnet.Packet>(_send_queue);
            _send_queue.Clear();
            _send_queue_idx = 0;

			Debug.Log("[Session.Send_Reconnect_Req] send reconnect req(msg_seq:" + packet.msg_seq + ", disconnect_state:" + _disconnectState.ToString() + ", json:" + json + ")");
            SendMsg(packet);

            foreach (var itr in tmp)
			{
				itr.read_index = 0;
				_send_queue.Add(itr);
			}
		}
		
		[System.Serializable] 
		class Msg_Reconnect_Ans {
			public int error_code = 0;
			//public uint session_key = 0;
			//public string session_token = "";
			//public uint msg_seq = 0;
		}
		public class ReconnectHandler : IMsgHandler {
			private Session session;
			public ReconnectHandler(Session session) {
				this.session = session;
			}
			public override void OnRecvMsg(System.IO.MemoryStream buffer) {
				string json = System.Text.Encoding.UTF8.GetString(buffer.GetBuffer(), (int)buffer.Position, (int)buffer.Length);
				Msg_Reconnect_Ans ans = JsonUtility.FromJson<Msg_Reconnect_Ans>(json);
				if(0 != ans.error_code)
				{
					Debug.LogError ("[Session.ReconnectHandler.OnRecvMsg] reconnect fail(error_code:" + ans.error_code +")");
					session.Error(new Gamnet.Exception(ErrorCode.ReconnectFailError, "reconnect fail(error_code:" + ans.error_code +")"));
					return;
				}
				Debug.Log("[Session.ReconnectHandler.OnRecvMsg] recv reconnect ans(session_key:" + session._session_key + ", session_token:" + session._session_token + ", disconnect_state:" + session._disconnectState.ToString() +")");
				//session._session_key = ans.session_key;
				//session._session_token = ans.session_token;

                SessionEvent evt = null;
                switch (session._disconnectState)
                {
                    case DisconnectState.Pause:
                        evt = new ResumeEvent(session);
                        break;
                    case DisconnectState.Handover:
                        evt = new ReconnectEvent(session);
                        break;
                    case DisconnectState.Closed:
                        break;
                }

                if (null != evt)
                {
                    session._event_queue.Add(evt); // already locked
                }
				session._disconnectState = DisconnectState.Invalid;
			}
		}
		
		void Send_HeartBeat_Req()
		{
            Reconnect();
			Gamnet.Packet packet = new Gamnet.Packet ();
			packet.length = Packet.HEADER_SIZE;
            packet.msg_seq = 0; // ++_send_seq;
			packet.msg_id = MsgID_CliSvr_HeartBeat_Req;
			packet.reliable = false;
			//Debug.Log("[Session.Send_HeartBeat_Req] send heartbeat req(msg_seq:" + packet.msg_seq + ")");
			SendMsg (packet);
		}
		
		[System.Serializable]
		class Msg_HeartBeat_Ans {
			public int error_code = 0;
			public uint msg_seq = 0;
		}
		public class HeartBeatHandler : IMsgHandler {
			private Session session;
			public HeartBeatHandler(Session session) {
				this.session = session;
			}
			public override void OnRecvMsg(System.IO.MemoryStream buffer) {
				string json = System.Text.Encoding.UTF8.GetString(buffer.GetBuffer(), (int)buffer.Position, (int)buffer.Length);
				Msg_HeartBeat_Ans ans = JsonUtility.FromJson<Msg_HeartBeat_Ans>(json);
				if(0 != ans.error_code)
				{
					Debug.LogError ("[Session.HeartBeatHandler.OnRecvMsg] heart beat message error(error_code:" + ans.error_code +")");
					session.Error(new Gamnet.Exception(ErrorCode.HeartbeatFailError, "heart beat message error(error_code:" + ans.error_code + ")"));
					return;
				}
				//Debug.Log("[Session.HeartBeatHandler.OnRecvMsg] recv heartbeat ans(msg_seq:" + ans.msg_seq + ")");
				session.RemoveSentPacket(ans.msg_seq);
			}
		}

        [System.Serializable]
        class Msg_ReliableAck_Ntf {
            public uint ack_seq = 0;
        }
        void Send_ReliableAck_Ntf(uint msgSEQ)
        {
            Reconnect();
            Msg_ReliableAck_Ntf ntf = new Msg_ReliableAck_Ntf();
            ntf.ack_seq = msgSEQ;
            string json = JsonUtility.ToJson(ntf);
            byte[] data = System.Text.Encoding.UTF8.GetBytes(json);

            Gamnet.Packet packet = new Gamnet.Packet();
            packet.length = (ushort)(Packet.HEADER_SIZE + data.Length);
            packet.msg_seq = 0;// ++_send_seq;
            packet.msg_id = MsgID_CliSvr_ReliableAck_Ntf;
            packet.reliable = false;
            packet.Append(data);

            SendMsg(packet);
        }

        [System.Serializable]
		class Msg_Kickout_Ntf {
			public int error_code = 0;
		}
		public class KickoutHandler : IMsgHandler {
			private Session session;
			public KickoutHandler(Session session) {
				this.session = session;
			}
			public override void OnRecvMsg(System.IO.MemoryStream buffer) {
				string json = System.Text.Encoding.UTF8.GetString(buffer.GetBuffer(), (int)buffer.Position, (int)buffer.Length);
				Msg_Kickout_Ntf ntf = JsonUtility.FromJson<Msg_Kickout_Ntf>(json);
				Debug.Log("[Session.KickoutHandler.OnRecvMsg] recv kickout ntf(error_code:" + ntf.error_code + ")");
				session.Error(new Gamnet.Exception(ErrorCode.DuplicateConnectionError));
				session.Close();
			}
		}
        
        void Send_Close_Req()
        {
            if (ConnectionState.Disconnected == _state)
            {
                return;
            }
            Gamnet.Packet packet = new Gamnet.Packet();
            packet.length = Packet.HEADER_SIZE;
            packet.msg_seq = 0; // ++_send_seq;
            packet.msg_id = MsgID_CliSvr_Close_Req;
            packet.reliable = false;
			Debug.Log("[Session.Send_Close_Req]");
			SendMsg(packet);
        }

        public class CloseHandler : IMsgHandler
        {
            private Session session;
            public CloseHandler(Session session)
            {
                this.session = session;
            }
            public override void OnRecvMsg(System.IO.MemoryStream buffer)
            {
                session.Disconnect();
            }
        }
    }
}