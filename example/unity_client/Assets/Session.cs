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
		private Dictionary<int, List<System.Timers.Timer>> timers = new Dictionary<int, List<System.Timers.Timer>>();
		public delegate void OnTimeout();

		public TimeoutMonitor()
		{
		}

		public void SetTimeout(int msg_id, int timeout, System.Timers.ElapsedEventHandler timeout_callback)
		{
			System.Timers.Timer timer = new System.Timers.Timer();
			timer.Interval = timeout * 1000;
			timer.AutoReset = false;
			timer.Elapsed += timeout_callback;
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
	public class SyncQueue<T> 
	{
		private object _syncObject = new object();
        private List<T> items = new List<T>();

		public int Count()
		{
			lock (_syncObject)
				return items.Count;
		}

		public void PushBack(T item)
		{
			lock (_syncObject)
			{
				items.Add(item);
			}
		}

        public void PushFront(T item)
        {
            lock (_syncObject)
            {
                items.Insert(0, item);
            }
        }

		public T PopFront()
		{
            lock (_syncObject)
            {
                T item = items[0];
                items.RemoveAt(0);
                return item;
            }
		}

        public T this[int index]
        {
            get {
                return items[index];
            }
        }

        public void Clear()
		{
            lock (_syncObject)
            {
                items.Clear();
            }
		}

        public List<T> Copy()
        {
            lock (_syncObject)
            {
                List<T> copy = new List<T>(items);
                return copy;
            }
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
		private int connectTimeout = 5000;
		private TimeoutMonitor timeoutMonitor = null;

		private Gamnet.Buffer recvBuffer = new Gamnet.Buffer();
		private Dictionary<int, Action<Gamnet.Buffer>> handlers = new Dictionary<int, Action<Gamnet.Buffer>>();

        private object syncObject = new object();
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
                    List<Gamnet.Buffer> sendQueue = session.sendQueue.Copy();
                    session.sendQueue.Clear();
					session.onReconnect ();
                    foreach (var itr in sendQueue)
                    {
                        session.sendQueue.PushBack(itr);
                    }
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
        public class LogEvent : SessionEvent
        {
            private string text;
            public LogEvent(Session session, string text) : base(session) {
                this.text = text;
            }
            public override void Event()
            {
                UILog.Write(text);
            }
        }
        
        public void Connect(string host, int port, int millisecondTimeout = 5000)
        {
            try
            {
				connectTimeout = millisecondTimeout;
                timeoutMonitor = new TimeoutMonitor();
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
				Log("Connect(" + _endPoint.ToString() +")");
                Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                IAsyncResult result = socket.BeginConnect(_endPoint, new AsyncCallback(Callback_OnConnect), socket);
				bool success = result.AsyncWaitHandle.WaitOne(millisecondTimeout, true );
				if (false == success )
				{
					socket.Close();
					throw new ApplicationException("Failed to connect server.");
				}
            }
            catch (SocketException e)
            {
                Log("Connect.Exception");
                Error(e);
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
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 10000);
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 10000);
                ConnectEvent evt = new ConnectEvent(this);
				eventQueue.PushBack(evt);
				Receive();
			}
			catch (System.Exception e)
			{
                Log("Callback_OnConnect.Exception");
                Error(e);
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

			Log("Reconnect(" + _endPoint.ToString() +")");
            IAsyncResult result = socket.BeginConnect(_endPoint, new AsyncCallback(Callback_OnReconnect), socket);
			bool success = result.AsyncWaitHandle.WaitOne(connectTimeout, true);
            if (false == success)
            {
                socket.Close();
                throw new ApplicationException("Failed to connect server.");
            }
        }
		private void Callback_OnReconnect(IAsyncResult result)
		{
			try
			{
				_socket = (Socket)result.AsyncState;
				_socket.EndConnect(result);
				_socket.ReceiveBufferSize = Gamnet.Buffer.BUFFER_SIZE;
				_socket.SendBufferSize = Gamnet.Buffer.BUFFER_SIZE;
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 10000);
                //_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 10000);
                ReconnectEvent evt = new ReconnectEvent(this);
				eventQueue.PushBack(evt);
				Receive();
			}
			catch (SocketException e)
			{
                Log("Callback_OnReconnect.Exception");
                Error(e);
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
            catch (SocketException e)
            {
                Error(e);
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
                eventQueue.PushBack(evt);
                Receive();
            }
            catch (SocketException e)
            {
				if (ConnectionState.Disconnected == state) {
					return;
				}
				Log("Callback_OnReceive.Exception");
                Error(e);
                Close();
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
            eventQueue.PushBack(evt);
        }
        public void Close()
        {
            if (ConnectionState.Disconnected == state)
            {
                return;
            }
            try
            {
				Log("Close()");
				state = ConnectionState.Disconnected;
                _socket.BeginDisconnect(false, new AsyncCallback(Callback_OnClose), _socket);
            }
            catch (SocketException e)
            {
                Log("Close.Exception");
                Error(e);
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
				eventQueue.PushBack(evt);
			}
			catch (SocketException e)
			{
                Log("Callback_OnClose.Exception");
                Error(e);
			}
		}
        
		public TimeoutMonitor SendMsg(object msg)
		{
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

                Gamnet.Buffer buffer = new Gamnet.Buffer();
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

                lock (syncObject)
                {
                    Log("SendMsg.sendQueue.PushBack(msg_id:" + msgID + ")");

                    sendQueue.PushBack(buffer);
                    if (1 == sendQueue.Count() && ConnectionState.Connected == state)
                    {
                        Log("SendMsg.BeginSend(msg_id:" + msgID + ")");
                        _socket.BeginSend(sendQueue[0].data, 0, sendQueue[0].Size(), 0, new AsyncCallback(Callback_OnSend), buffer);
                    }
                }
			}
			catch (System.Exception e)
			{
				Log("SendMsg.Exception");
                Error(e);
			}
			return timeoutMonitor;
		}
        
        private void Callback_OnSend(IAsyncResult result)
		{
            try
			{
                lock (syncObject)
                {
					Log("Callback_OnSend(queue size:" + sendQueue.Count() + ", connection:" + _socket.Connected.ToString() + ")");
                    Gamnet.Buffer buffer = sendQueue[0];
					int writedBytes = _socket.EndSend(result);
                    buffer.readIndex += writedBytes;
                    if (buffer.Size() > 0)
                    {
                        Gamnet.Buffer newBuffer = new Gamnet.Buffer(buffer);
                        sendQueue.PushFront(newBuffer);
                        Log("Callback_OnSend.BeginSend.newBuffer");
                        _socket.BeginSend(newBuffer.data, 0, newBuffer.Size(), 0, new AsyncCallback(Callback_OnSend), newBuffer);
                        return;
                    }

                    sendQueue.PopFront();
                    if (0 < sendQueue.Count())
                    {
                        Log("Callback_OnSend.BeginSend.QueuedMsg(queue size:" + sendQueue.Count() +")");
                        _socket.BeginSend(sendQueue[0].data, 0, sendQueue[0].Size(), 0, new AsyncCallback(Callback_OnSend), sendQueue[0]);
						return;
                    }
                }
                Log("Callback_OnSend.sendQueue.Count:" + sendQueue.Count());
			}
            catch (SocketException e)
			{
                Log("Callback_OnSend.Exception");
				Error(e);
			}
		}

        private void Log(string text)
        {
            LogEvent evt = new LogEvent(this, text);
            eventQueue.PushBack(evt);
        }
        public void Update()
        {
            while (0 < eventQueue.Count())
            {
				SessionEvent evt = eventQueue.PopFront();
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