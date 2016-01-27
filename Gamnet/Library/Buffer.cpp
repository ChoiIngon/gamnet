/*
 * Buffer.cpp
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#include "Buffer.h"
namespace Gamnet {

int Buffer::MAX_SIZE = 0x0000ffff;

Buffer::Buffer(uint16_t size) : writeCursor_(0), readCursor_(0), bufSize_(size), buf_(NULL)
{
	buf_ = new char[bufSize_];
	if(NULL == buf_)
	{
		throw Exception(ENOMEM, "not enough space(need:", size," bytes)");
	}
	memset(buf_, 0, bufSize_);
}

Buffer::~Buffer()
{
	if(NULL != buf_)
	{
		delete [] buf_;
		buf_ = NULL;
	}
}

size_t Buffer::Available() const
{
	return bufSize_ - (writeCursor_ - readCursor_);
}

size_t Buffer::Capacity() const
{
	return bufSize_;
}

void Buffer::Clear()
{
	writeCursor_ = 0;
	readCursor_ = 0;
}

void Buffer::RemoveReadBuffer()
{
	if(0 >= readCursor_)
	{
		return;
	}
	if(0 < writeCursor_-readCursor_)
	{
		std::memcpy(buf_, buf_+readCursor_, writeCursor_ - readCursor_);
	}
	writeCursor_ -= readCursor_;
    readCursor_ = 0;
}

void Buffer::Append(const char* buf, uint16_t size)
{
	if(Available() < size)
	{
		throw Exception(ENOMEM, "not enough space(need:", size," bytes, left:", Available(), " bytes)");
	}

	if(size > bufSize_ - writeCursor_)
	{
		RemoveReadBuffer();
	}

	std::memcpy(buf_+writeCursor_, buf, size);
	writeCursor_ += size;
}

void Buffer::Copy(const Buffer& buffer)
{
	readCursor_ = 0;
	writeCursor_ = 0;
	if(Available() < buffer.Size())
	{
		Resize(buffer.Size());
	}
	std::memcpy(buf_, buffer.ReadPtr(), buffer.Size());
	writeCursor_ += buffer.Size();
}

void Buffer::Remove(uint16_t size)
{
	readCursor_ += size;
	if(readCursor_ > writeCursor_)
	{
		readCursor_ = writeCursor_;
	}
	if(readCursor_ == writeCursor_)
	{
		writeCursor_ = 0;
		readCursor_ = 0;
	}
}

size_t Buffer::Size() const
{
	return writeCursor_ - readCursor_;
}

const char* Buffer::ReadPtr() const
{
	return buf_+readCursor_;
}

char* Buffer::WritePtr()
{
	RemoveReadBuffer();
	if(writeCursor_ >= bufSize_)
	{
		return NULL;
	}
	return buf_+writeCursor_;
}

void Buffer::Resize(uint16_t size)
{
	char* oldBuf = buf_;
	buf_ = new char[size];
	bufSize_ = size;
	memset(buf_, 0, bufSize_);
	if(0 < Size())
	{
		std::memcpy(buf_, oldBuf+readCursor_, Size());
	}

	writeCursor_ -= readCursor_;
	readCursor_ = 0;
	delete [] oldBuf;
}

}


