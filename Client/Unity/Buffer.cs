using System;
using System.Collections.Generic;
using System.IO;

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

        public void Append(byte[] src)
        {
            if (0 == src.Length)
            {
                return;
            }
            System.Buffer.BlockCopy(src, 0, data, writeIndex, src.Length);
            writeIndex += src.Length;
        }
        public void Append(MemoryStream ms)
        {
            if (0 == ms.GetBuffer().Length)
            {
                return;
            }
            System.Buffer.BlockCopy(ms.GetBuffer(), 0, data, writeIndex, ms.GetBuffer().Length);
            writeIndex += ms.GetBuffer().Length;
        }
        public void Append(Buffer src)
        {
            if (0 == src.Size())
            {
                return;
            }
            System.Buffer.BlockCopy(src.data, src.readIndex, data, writeIndex, src.Size());
            writeIndex += src.Size();
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
            Gamnet.Buffer buffer = new Gamnet.Buffer();
            buffer.Copy(lhs);
            buffer.Append(rhs);
            return buffer;
        }
    }
}