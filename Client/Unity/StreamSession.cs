using System;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;

namespace Gamnet
{
    public class TimeoutMonitor
    {
        public Session session;
        public Dictionary<int, List<System.Timers.Timer>> timers = new Dictionary<int, List<System.Timers.Timer>>();
        
        public class TimeoutEvent : Session.NetworkEvent
        {
            public int msgID = 0;

            public TimeoutEvent(Session session) : base(session)
            {
            }
            
            public override void Event()
            {
                ((StreamSession)session).OnTimeout(msgID);
            }
        }

        public TimeoutMonitor(Session session)
        {
            this.session = session;
        }

        public void SetTimeout(int msgID, int timeout)
        {
            System.Timers.Timer timer = new System.Timers.Timer();
            timer.Interval = timeout * 1000;
            timer.AutoReset = false;
            timer.Elapsed += delegate
            {
                TimeoutEvent evt = new TimeoutEvent(session);
                evt.msgID = msgID;
                session.eventQueue.Enqueue(evt);
            };
            timer.Start();

            if (false == timers.ContainsKey(msgID))
            {
                timers.Add(msgID, new List<System.Timers.Timer>());
            }
            timers[msgID].Add(timer);
        }

        public void UnsetTimeout(int msgID)
        {
            if (false == timers.ContainsKey(msgID))
            {
                return;
            }

            if (0 == timers[msgID].Count)
            {
                return;
            }
            timers[msgID].RemoveAt(0);
        }
    }

    public class StreamSession : Session
    {
        private Gamnet.Buffer recvBuffer = new Gamnet.Buffer();
        private TimeoutMonitor timeoutMonitor;
        public Dictionary<int, Action<Gamnet.Buffer>> handlers = new Dictionary<int, Action<Gamnet.Buffer>>();

        public StreamSession()
        {
            timeoutMonitor = new TimeoutMonitor(this);
        }
        // length<2byte> | msg_id<4byte> | body
        public const int PACKET_SIZE_OFFSET = 0;
        public const int MSGID_OFFSET = 2;
        public const int PACKET_HEADER_SIZE = 6;

        public override void OnConnect() { }
        public override void OnReconnect() { }
        public override void OnClose() { }
        public override void OnError(System.Exception e) { }
        public virtual void OnTimeout(int msgID) { }
        public class TimeoutInfo
        {
            public int msgID;
            public int timeout;
        }
        public virtual TimeoutMonitor SendMsg(object msg)
        {
            try
            {
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

                byte[] buffer = new byte[packetLength];
                byte[] length_buf = BitConverter.GetBytes(packetLength);

                buffer[0] = length_buf[0];
                buffer[1] = length_buf[1];

                byte[] msgid_buf = BitConverter.GetBytes(msgID);

                buffer[2] = msgid_buf[0];
                buffer[3] = msgid_buf[1];
                buffer[4] = msgid_buf[2];
                buffer[5] = msgid_buf[3];

                System.Array.Copy(ms.GetBuffer(), 0, buffer, PACKET_HEADER_SIZE, dataLength);

                AsyncSend(buffer);
            }
            catch (System.Exception error)
            {
                Error(error);
                Close();
            }
            return timeoutMonitor;
        }
        public override void OnReceive(Gamnet.Buffer buf)
        {
            recvBuffer += buf;
            while (recvBuffer.Size() >= PACKET_HEADER_SIZE)
            {
                ushort packetLength = BitConverter.ToUInt16(recvBuffer.data, recvBuffer.readIndex + PACKET_SIZE_OFFSET);
                if (packetLength > Gamnet.Buffer.BUFFER_SIZE)
                {
                    throw new Exception(string.Format("The packet length is greater than the buffer max length."));
                }

                if (packetLength > recvBuffer.Size())
                { // not enough
                    return;
                }

                int msgID = BitConverter.ToInt32(recvBuffer.data, recvBuffer.readIndex + MSGID_OFFSET);
                if (false == handlers.ContainsKey(msgID))
                {
                    ErrorEvent evt = new ErrorEvent(this);
                    evt.exception = new System.Exception("can't find registered msg(id:" + msgID + ")");
                    eventQueue.Enqueue(evt);
                    return;
                }

                recvBuffer.readIndex += PACKET_HEADER_SIZE;
                Action<Gamnet.Buffer> handler = handlers[msgID];

                try
                {
                    timeoutMonitor.UnsetTimeout(msgID);
                    handler(recvBuffer);
                }
                catch (System.Exception) { }
            }
        }
        public void RegisterHandler(int msgID, Action<Gamnet.Buffer> handler)
        {
            handlers[msgID] = handler;
        }

        
    }
}