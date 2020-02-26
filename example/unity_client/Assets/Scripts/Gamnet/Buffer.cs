using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Gamnet {
public class Buffer	{
	public const int BUFFER_SIZE = 65535;
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
		public Buffer(System.IO.MemoryStream src)	{ Copy(src); }

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
		public void Copy(System.IO.MemoryStream src) {
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
}