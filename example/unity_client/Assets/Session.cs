using UnityEngine;
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;

namespace Gamnet
{
	public class Buffer
	{
		public static int BUFFER_SIZE = 8192;
		public static void BlockCopy(Buffer src, Buffer dest)
		{
			System.Buffer.BlockCopy(src.data, src.readIndex, dest.data, 0, src.Size());
			dest.readIndex = 0;
			dest.writeIndex = src.Size();
		}

		public byte[] data = new byte[BUFFER_SIZE];

		public int readIndex = 0;
		public int writeIndex = 0;

		public Buffer() {}
		public Buffer(byte[] src)
		{
			Copy(src);
		}

		public Buffer(MemoryStream ms)
		{
			Copy(ms);
		}

		public Buffer(Buffer src)
		{
			Copy(src);
		}

		public int Size()
		{
			return writeIndex - readIndex;
		}

		public int Available()
		{
			return BUFFER_SIZE - writeIndex;
		}

		public void Copy(byte[] src)
		{
			readIndex = 0;
			writeIndex = 0;
			Append(src);
		}
		public void Copy(MemoryStream ms)
		{
			readIndex = 0;
			writeIndex = 0;
			Append(ms);
		}
		public void Copy(Buffer src)
		{
			readIndex = 0;
			writeIndex = 0;
			Append(src);
		}

		public void Append(System.IO.MemoryStream src)
		{
			if (0 == src.Position)
			{
				return;
			}
			System.Buffer.BlockCopy(src.GetBuffer(), 0, data, writeIndex, (int)src.Position);
			writeIndex += (int)src.Position;
		}
		public void Append(Buffer src)
		{
			if (0 == src.Size())
			{
				return;
			}
			System.Buffer.BlockCopy(src.data, 0, data, writeIndex, src.Size());
			writeIndex += src.Size();
		}
		public void Append(byte[] src)
		{
			if (0 == src.Length)
			{
				return;
			}
			System.Buffer.BlockCopy(src, 0, data, writeIndex, src.Length);
			writeIndex += src.Length;
		}

		public static implicit operator System.IO.MemoryStream(Gamnet.Buffer src)  // explicit byte to digit conversion operator
		{
			System.IO.MemoryStream ms = new System.IO.MemoryStream();
			ms.Write(src.data, src.readIndex, src.Size());
			ms.Seek(0, System.IO.SeekOrigin.Begin);
			return ms;
		}
		public static implicit operator byte[] (Gamnet.Buffer src)
		{
			byte[] dest = new byte[src.Size()];
			System.Buffer.BlockCopy(src.data, src.readIndex, dest, 0, src.Size());
			return dest;
		}
		public static Gamnet.Buffer operator + (Gamnet.Buffer lhs, Gamnet.Buffer rhs)
		{
			Gamnet.Buffer buffer = new Gamnet.Buffer (lhs);
			buffer.Append(rhs);
			return buffer;
		}
	}
	public class TimeoutMonitor
	{
		private Session session;
		private Dictionary<int, List<System.Timers.Timer>> timers = new Dictionary<int, List<System.Timers.Timer>>();

		public class TimeoutEvent : Session.SessionEvent
		{
			public int msgID = 0;
			public TimeoutEvent(Session session) : base(session)
			{
			}

			public override void Event()
			{
				if (null != session.onError) {
					session.onError (new System.TimeoutException (msgID.ToString ()));
				}
			}
		}

		public TimeoutMonitor(Session session)
		{
			this.session = session;
		}

		public void SetTimeout(int msg_id, int timeout)
		{
			System.Timers.Timer timer = new System.Timers.Timer();
			timer.Interval = timeout * 1000;
			timer.AutoReset = false;
			timer.Elapsed += delegate
			{
				TimeoutEvent evt = new TimeoutEvent(session);
				evt.msgID = msg_id;
				session.eventQueue.Enqueue(evt);
			};
			timer.Start();

			if (false == timers.ContainsKey(msg_id))
			{
				timers.Add(msg_id, new List<System.Timers.Timer>());
			}
			timers[msg_id].Add(timer);
		}
		public void UnsetTimeout(int msg_id)
		{
			if (false == timers.ContainsKey(msg_id))
			{
				return;
			}

			if (0 == timers[msg_id].Count)
			{
				return;
			}
			timers[msg_id].RemoveAt(0);
		}
	}
	public class SyncQueue<T> : Queue<T>
	{
		private object _syncObject = new object();
		public int GetCount()
		{
			lock (_syncObject)
				return base.Count;
		}

		public new void Enqueue(T container)
		{
			lock (_syncObject)
			{
				base.Enqueue(container);
			}
		}

		public new T Dequeue()
		{
			lock (_syncObject)
				return base.Dequeue();
		}

		public new void Clear()
		{
			lock (_syncObject)
				base.Clear();
		}
	};

    public class Session
    {
		// length<2byte> | msg_id<4byte> | body
		public const int PACKET_SIZE_OFFSET = 0;
		public const int MSGID_OFFSET = 2;
		public const int PACKET_HEADER_SIZE = 6;

        public enum ConnectionState
        {
            Disconnected,
            OnConnecting,
            Connected
        }

        private Socket _socket = null;
        private IPEndPoint _endPoint = null;

		private TimeoutMonitor timeoutMonitor;

		private Gamnet.Buffer recvBuffer = new Gamnet.Buffer();
		private Dictionary<int, Action<Gamnet.Buffer>> handlers = new Dictionary<int, Action<Gamnet.Buffer>>();

		public SyncQueue<SessionEvent> eventQueue = new SyncQueue<SessionEvent>();
		public SyncQueue<Gamnet.Buffer> sendQueue = new SyncQueue<Gamnet.Buffer>(); // 바로 보내지 못하고 
        public ConnectionState state = ConnectionState.Disconnected;

		public delegate void OnConnect();
		public delegate void OnReconnect();
		public delegate void OnClose();
		public delegate void OnError(System.Exception e);

		public OnConnect onConnect;
		public OnReconnect onReconnect;
		public OnClose onClose;
		public OnError onError;

        public abstract class SessionEvent
        {
            protected Session session;
			public SessionEvent(Session session)
            {
                this.session = session;
            }
            public abstract void Event();
        };
		public class ConnectEvent : SessionEvent
        {
            public ConnectEvent(Session session) : base(session) {}
            public override void Event()
            {
                session.state = ConnectionState.Connected;
                session.sendQueue.Clear();
				if (null != session.onConnect) {
					session.onConnect ();
				}
            }
        }
		public class ReconnectEvent : SessionEvent
        {
            public ReconnectEvent(Session session) : base(session) { }
            public override void Event()
            {
                session.state = ConnectionState.Connected;
				if (null != session.onReconnect) {
					session.onReconnect ();
				}
                while (0 < session.sendQueue.GetCount())
                {
                    session.PostSend(session.sendQueue.Dequeue());
                }
            }
        }
        public class ErrorEvent : SessionEvent
        {
            public ErrorEvent(Session session) : base(session) { }
            public System.Exception exception;
            public override void Event()
            {
				if (null != session.onError) {
					session.onError (exception);
				}
            }
        }
		public class CloseEvent : SessionEvent
		{
			public CloseEvent(Session session) : base(session) { }
			public override void Event()
			{
				session.state = ConnectionState.Disconnected;
				if (null != session.onClose) {
					session.onClose ();
				}
			}
		}
        public class ReceiveEvent : SessionEvent
        {
            public ReceiveEvent(Session session) : base(session) { }
            public Gamnet.Buffer buffer;
            public override void Event()
            {
                session.OnReceive(buffer);
            }
        }
        
        public void Connect(string host, int port, int millisecondTimeout = 5000)
        {
            try
            {
				timeoutMonitor = new TimeoutMonitor(this);
                state = ConnectionState.OnConnecting;
				_socket = null;
                IPAddress ip = null;
                try
                {
                    ip = IPAddress.Parse(host);
                }
                catch (System.FormatException)
                {
                    IPHostEntry hostEntry = Dns.GetHostEntry(host);
                    if (hostEntry.AddressList.Length > 0)
                    {
                        ip = hostEntry.AddressList[0];
                    }
                }
                _endPoint = new IPEndPoint(ip, port);
                Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                IAsyncResult result = socket.BeginConnect(_endPoint, new AsyncCallback(Callback_OnConnect), socket);
				bool success = result.AsyncWaitHandle.WaitOne(millisecondTimeout, true );
				if (false == success )
				{
					socket.Close();
					throw new ApplicationException("Failed to connect server.");
				}
            }
            catch (System.Exception error)
            {
                Error(error);
            }
        }
		private void Callback_OnConnect(IAsyncResult result)
		{
			try
			{
				_socket = (Socket)result.AsyncState;
				_socket.EndConnect(result);
				_socket.ReceiveBufferSize = Gamnet.Buffer.BUFFER_SIZE;
				_socket.SendBufferSize = Gamnet.Buffer.BUFFER_SIZE;

				ConnectEvent evt = new ConnectEvent(this);
				eventQueue.Enqueue(evt);
				Receive();
			}
			catch (System.Exception error)
			{
				Error(error);
				Close();
			}
		}

        private void Reconnect()
        {
            if (ConnectionState.Disconnected != state)
            {
                return;
            }
            state = ConnectionState.OnConnecting;
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.BeginConnect(_endPoint, new AsyncCallback(Callback_OnReconnect), socket);
        }
		private void Callback_OnReconnect(IAsyncResult result)
		{
			try
			{
				_socket = (Socket)result.AsyncState;
				_socket.EndConnect(result);
				_socket.ReceiveBufferSize = Gamnet.Buffer.BUFFER_SIZE;
				_socket.SendBufferSize = Gamnet.Buffer.BUFFER_SIZE;
				ReconnectEvent evt = new ReconnectEvent(this);
				eventQueue.Enqueue(evt);
				Receive();
			}
			catch (System.Exception error)
			{
				Error(error);
				Close();
			}
		}

        private void Receive()
        {
            try
            {
				Gamnet.Buffer buffer = new Gamnet.Buffer();
                _socket.BeginReceive(buffer.data, 0, Gamnet.Buffer.BUFFER_SIZE, 0, new AsyncCallback(Callback_OnReceive), buffer);
            }
            catch (System.Exception error)
            {
                Error(error);
                Close();
            }
        }
		private void Callback_OnReceive(IAsyncResult result)
		{
			try
			{
				Gamnet.Buffer buffer = (Gamnet.Buffer)result.AsyncState;
				int recvBytes = _socket.EndReceive(result);

				if (0 == recvBytes)
				{
					Close();
					return;
				}
				buffer.writeIndex += recvBytes;

				ReceiveEvent evt = new ReceiveEvent(this);
				evt.buffer = buffer;
				eventQueue.Enqueue(evt);
				Receive();
			}
			catch (System.Exception error)
			{
				Error(error);
				Close ();
			}
		}
		public void OnReceive(Gamnet.Buffer buf)
		{
			recvBuffer += buf;
			while (recvBuffer.Size() >= PACKET_HEADER_SIZE)
			{
				ushort packetLength = BitConverter.ToUInt16(recvBuffer.data, recvBuffer.readIndex + PACKET_SIZE_OFFSET);
				if (packetLength > Gamnet.Buffer.BUFFER_SIZE)
				{
					throw new System.Exception(string.Format("The packet length is greater than the buffer max length."));
				}

				if (packetLength > recvBuffer.Size()) // not enough
				{ 
					return;
				}

				int msgID = BitConverter.ToInt32(recvBuffer.data, recvBuffer.readIndex + MSGID_OFFSET);
				if (false == handlers.ContainsKey(msgID))
				{
					throw new System.Exception ("can't find registered msg(id:" + msgID + ")");
				}
				recvBuffer.readIndex += PACKET_HEADER_SIZE;
				Action<Gamnet.Buffer> handler = handlers[msgID];

				try
				{
					handler(recvBuffer);
					timeoutMonitor.UnsetTimeout(msgID);
				}
				catch (System.Exception e) {
					Error (e);
				}
				recvBuffer.readIndex += packetLength - PACKET_HEADER_SIZE;
			}
		}

        public void Error(System.Exception e)
        {
            ErrorEvent evt = new ErrorEvent(this);
            evt.exception = e;
            eventQueue.Enqueue(evt);
        }
        public void Close()
        {
            try
            {
				state = ConnectionState.Disconnected;
                _socket.BeginDisconnect(false, new AsyncCallback(Callback_OnClose), _socket);
            }
            catch (System.Exception error)
            {
                Error(error);
            }
        }
		private void Callback_OnClose(IAsyncResult result)
		{
			try
			{
				_socket.EndDisconnect(result);
				//_socket.Shutdown(SocketShutdown.Both);
				_socket.Close();
				CloseEvent evt = new CloseEvent(this);
				eventQueue.Enqueue(evt);
			}
			catch (System.Exception error)
			{
				Error(error);
			}
		}
        
		public TimeoutMonitor SendMsg(object msg)
		{
			Gamnet.Buffer buffer = null;
			try
			{
				Reconnect();

				System.IO.MemoryStream ms = new System.IO.MemoryStream();
				System.Type type = msg.GetType();
				type.GetMethod("Store").Invoke(msg, new object[] { ms });
				System.Reflection.FieldInfo fi = type.GetField("MSG_ID");

				int msgID = (int)fi.GetValue(msg);
				int dataLength = (int)(type.GetMethod("Size").Invoke(msg, null));
				ushort packetLength = (ushort)(PACKET_HEADER_SIZE + dataLength);

				if (packetLength > Gamnet.Buffer.BUFFER_SIZE)
				{
					throw new System.Exception(string.Format("Overflow the send buffer max size : {0}", packetLength));
				}

				buffer = new Gamnet.Buffer();
				byte[] bufLength = BitConverter.GetBytes(packetLength);
				byte[] bufMsgID = BitConverter.GetBytes(msgID);
				buffer.data[0] = bufLength[0];
				buffer.data[1] = bufLength[1];
				buffer.data[2] = bufMsgID[0];
				buffer.data[3] = bufMsgID[1];
				buffer.data[4] = bufMsgID[2];
				buffer.data[5] = bufMsgID[3];
				buffer.writeIndex = PACKET_HEADER_SIZE;
				buffer.Append(ms);

				PostSend(buffer);
			}
			catch (System.Exception e)
			{
				if (null != buffer) {
					sendQueue.Enqueue(buffer);
				}
				Error(e);
			}
			return timeoutMonitor;
		}
		private void PostSend(Gamnet.Buffer buffer)
        {
            if (ConnectionState.Connected == state)
            {
		        _socket.BeginSend(buffer.data, 0, buffer.Size(), 0, new AsyncCallback(Callback_OnSend), buffer);
            }
            else
            {
                sendQueue.Enqueue(buffer);
            }
        }
		private void Callback_OnSend(IAsyncResult result)
		{
			try
			{
				Gamnet.Buffer buffer = (Gamnet.Buffer)result.AsyncState;
				int writedBytes = _socket.EndSend(result);
				buffer.readIndex += writedBytes;
				if (buffer.Size() > 0)
				{
					Gamnet.Buffer newBuffer = new Gamnet.Buffer(buffer);
					PostSend(newBuffer);
				}
			}
			catch (System.Exception error)
			{
				Error(error);
				Close();
			}
		}
			
        public void Update()
        {
            while (0 < eventQueue.GetCount())
            {
				SessionEvent evt = eventQueue.Dequeue();
                evt.Event();
            }
        }
		public void RegisterHandler(int msg_id, Action<Gamnet.Buffer> handler)
		{
			handlers[msg_id] = handler;
		}
		public void UnregisterHandler(int msg_id)
		{
			handlers.Remove (msg_id);
		}
    }
}