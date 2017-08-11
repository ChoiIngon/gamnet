using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;

namespace Gamnet
{
    public abstract class Session
    {
        public enum ConnectionState
        {
            Disconnected,
            OnConnecting,
            Connected
        }
        private Socket _socket = null;
        private IPEndPoint _endPoint = null;
        public SyncQueue<NetworkEvent> eventQueue = new SyncQueue<NetworkEvent>();
        private SyncQueue<byte[]> sendQueue = new SyncQueue<byte[]>(); // 바로 보내지 못하고 
        public ConnectionState state = ConnectionState.Disconnected;

        public abstract class NetworkEvent
        {
            protected Session session;
            public NetworkEvent(Session session)
            {
                this.session = session;
            }
            public abstract void Event();
        };

        public class ConnectEvent : NetworkEvent
        {
            public ConnectEvent(Session session) : base(session) {}
            public override void Event()
            {
                session.state = ConnectionState.Connected;
                session.sendQueue.Clear();
                session.OnConnect();
            }
        }
        public class ReconnectEvent : NetworkEvent
        {
            public ReconnectEvent(Session session) : base(session) { }
            public override void Event()
            {
                session.state = ConnectionState.Connected;
                session.OnReconnect();
                while (0 < session.sendQueue.GetCount())
                {
                    session.PostSend(session.sendQueue.Dequeue());
                }
            }
        }
        public class ErrorEvent : NetworkEvent
        {
            public ErrorEvent(Session session) : base(session) { }
            public System.Exception exception;
            public override void Event()
            {
                session.OnError(exception);
            }
        }
        public class ReceiveEvent : NetworkEvent
        {
            public ReceiveEvent(Session session) : base(session) { }
            public Gamnet.Buffer buffer;
            public override void Event()
            {
                session.OnReceive(buffer);
            }
        }
        public class CloseEvent : NetworkEvent
        {
            public CloseEvent(Session session) : base(session) { }
            public override void Event()
            {
                session.state = ConnectionState.Disconnected;
                session.OnClose();
            }
        }

        public class SendStateObject
        {
            public Gamnet.Buffer buffer = null;
        }

        public class ReceiveStateObject
        {
            public Socket socket = null;
            public Gamnet.Buffer buffer = null;
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

        public abstract void OnConnect();
        public abstract void OnReconnect();
        public abstract void OnClose();
        public abstract void OnError(System.Exception e);
        public abstract void OnReceive(Gamnet.Buffer buf);
        
        public virtual void Connect(string host, int port)
        {
            try
            {
                state = ConnectionState.OnConnecting;
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
                socket.BeginConnect(_endPoint, new AsyncCallback(Callback_OnConnect), socket);
            }
            catch (System.Exception error)
            {
                Error(error);
            }
        }
        private void Reconnect()
        {
            try
            {
                if (ConnectionState.Disconnected != state)
                {
                    return;
                }
                state = ConnectionState.OnConnecting;
                Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                socket.BeginConnect(_endPoint, new AsyncCallback(Callback_OnReconnect), socket);
            }
            catch (System.Exception error)
            {
                Error(error);
            }
        }
        private void Receive()
        {
            try
            {
                ReceiveStateObject obj = new ReceiveStateObject();
                obj.socket = _socket;
                obj.buffer = new Gamnet.Buffer();
                _socket.BeginReceive(obj.buffer.data, 0, Gamnet.Buffer.BUFFER_SIZE, 0, new AsyncCallback(Callback_OnReceive), obj);
            }
            catch (System.Exception error)
            {
                Error(error);
                Close();
            }
        }
        protected void Error(System.Exception e)
        {
            ErrorEvent evt = new ErrorEvent(this);
            evt.exception = e;
            eventQueue.Enqueue(evt);
        }
        public void Close()
        {
            try
            {
                _socket.Shutdown(SocketShutdown.Both);
                _socket.BeginDisconnect(false, new AsyncCallback(Callback_OnClose), null);
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
        private void Callback_OnReceive(IAsyncResult result)
        {
            try
            {
                ReceiveStateObject obj = (ReceiveStateObject)result.AsyncState;
                int recvBytes = _socket.EndReceive(result);

                if (0 == recvBytes)
                {
                    Close();
                    return;
                }
                obj.buffer.writeIndex += recvBytes;

                ReceiveEvent evt = new ReceiveEvent(this);
                evt.buffer = obj.buffer;
                eventQueue.Enqueue(evt);
                Receive();
            }
            catch (System.Exception error)
            {
                Error(error);
            }
        }
        private void Callback_OnSend(IAsyncResult result)
        {
            try
            {
                SendStateObject obj = (SendStateObject)result.AsyncState;
                int writedBytes = _socket.EndSend(result);
                obj.buffer.readIndex += writedBytes;
                if (obj.buffer.Size() > 0)
                {
                    Gamnet.Buffer newBuffer = new Gamnet.Buffer(obj.buffer);
                    AsyncSend(newBuffer.data);
                }
            }
            catch (System.Exception error)
            {
                Error(error);
                Close();
            }
        }
        private void Callback_OnClose(IAsyncResult result)
        {
            try
            {
                _socket.EndDisconnect(result);
                _socket.Close();
                _socket = null;
                CloseEvent evt = new CloseEvent(this);
                eventQueue.Enqueue(evt);
            }
            catch (System.Exception error)
            {
                Error(error);
            }
        }

        public void AsyncSend(byte[] buf)
        {
            try
            {
                Reconnect();
                PostSend(buf);
            }
            catch (System.Exception error)
            {
                Error(error);
                Close();
            }
        }
        private void PostSend(byte[] buf)
        {
            if (ConnectionState.Connected == state)
            {
                SendStateObject sendObj = new SendStateObject();
                sendObj.buffer = new Gamnet.Buffer(buf);
                _socket.BeginSend(sendObj.buffer.data, 0, sendObj.buffer.Size(), 0, new AsyncCallback(Callback_OnSend), sendObj);
            }
            else
            {
                sendQueue.Enqueue(buf);
            }
        }

        public void Update()
        {
            while (0 < eventQueue.GetCount())
            {
                NetworkEvent evt = eventQueue.Dequeue();
                evt.Event();
            }
        }
    }
}