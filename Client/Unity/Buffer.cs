using System;
using System.Collections.Generic;
using System.IO;

namespace Gamnet {
	public class Buffer
	{
		public static int BUFFER_SIZE = 8192;
		public byte[] data = null;
		
		public int readIndex = 0;
		public int writeIndex = 0;
		public Buffer() 
		{
			data = new byte[BUFFER_SIZE];
		}
        public Buffer(byte[] src)
		{
			data = new byte[BUFFER_SIZE];
            Copy(src);
		}
        public Buffer(MemoryStream ms)
        {
            data = new byte[BUFFER_SIZE];
            Copy(ms);
        }
        public Buffer(Buffer src)
        {
            data = new byte[BUFFER_SIZE];
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
			System.Buffer.BlockCopy (src, 0, data, 0, src.Length); 
			writeIndex = src.Length;
			readIndex = 0;
		}
		public void Copy(MemoryStream ms)
		{
			Array.Copy(ms.GetBuffer(), 0, data, readIndex, ms.GetBuffer().Length);
		}
		public void Copy(Buffer src)
		{
            if(0 == src.Size())
            {
                return;
            }
			System.Buffer.BlockCopy(src.data, src.readIndex, data, 0, src.Size());
			writeIndex = src.Size ();
			readIndex = 0;
		}
		public void Append(Buffer src)
		{
			System.Buffer.BlockCopy(src.data, src.readIndex, data, writeIndex, src.Size());
			writeIndex += src.Size ();
		}
		public static void BlockCopy (Buffer src, Buffer dest, int size)
		{
			System.Buffer.BlockCopy (src.data, src.readIndex, dest.data, 0, size);
			dest.readIndex = 0;
			dest.writeIndex = size;
		}
		public static implicit operator System.IO.MemoryStream(Gamnet.Buffer src)  // explicit byte to digit conversion operator
		{
			System.IO.MemoryStream ms = new System.IO.MemoryStream();
			ms.Write(src.data, src.readIndex, src.Size());
			ms.Seek(0, System.IO.SeekOrigin.Begin);
			return ms;
		}
        public static implicit operator byte[](Gamnet.Buffer src)
		{
			byte [] dest = new byte[src.Size ()];
			System.Buffer.BlockCopy (src.data, src.readIndex, dest, 0, src.Size ());
			return dest;
		}
        public static Gamnet.Buffer operator + (Gamnet.Buffer lhs, Gamnet.Buffer rhs)
        {
            Gamnet.Buffer buffer = new Gamnet.Buffer();
            buffer.Append(lhs);
            buffer.Append(rhs);
            return buffer;
        }
	}
}