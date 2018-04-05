/*
 * Buffer.cpp
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#include "Buffer.h"
#include "Pool.h"

namespace Gamnet {

int Buffer::MAX_SIZE = 0x0000ffff;

void Buffer::Swap(const std::shared_ptr<Buffer>& lhs, const std::shared_ptr<Buffer>& rhs)
{
	char* tmp_data = lhs->data;
	size_t tmp_write = lhs->writeCursor_;
	size_t tmp_read = lhs->readCursor_;
	size_t tmp_size = lhs->bufSize_;

	lhs->data = rhs->data;
	lhs->writeCursor_ = rhs->writeCursor_;
	lhs->readCursor_ = rhs->readCursor_;
	lhs->bufSize_ = rhs->bufSize_;

	rhs->data = tmp_data;
	rhs->writeCursor_ = tmp_write;
	rhs->readCursor_ = tmp_read;
	rhs->bufSize_ = tmp_size;
}

Buffer::Buffer(size_t size) : writeCursor_(0), readCursor_(0), bufSize_(size), data(NULL)
{
	data = new char[bufSize_];
	if(NULL == data)
	{
		throw Exception(ENOMEM, "not enough space(need:", size," bytes)");
	}
	memset(data, 0, bufSize_);
}

Buffer::~Buffer()
{
	if(NULL != data)
	{
		delete [] data;
		data = NULL;
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
		std::memcpy(data, data+readCursor_, writeCursor_ - readCursor_);
	}
	writeCursor_ -= readCursor_;
    readCursor_ = 0;
}

void Buffer::Append(const char* buf, size_t size)
{
	if(0 == size)
	{
		return;
	}
	if(Available() < size)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "not enough space(need:", size," bytes, left:", Available(), " bytes)");
	}

	if(size > bufSize_ - writeCursor_)
	{
		RemoveReadBuffer();
	}

	std::memcpy(data+writeCursor_, buf, size);
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
	std::memcpy(data, buffer.ReadPtr(), buffer.Size());
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
	return data +readCursor_;
}

char* Buffer::WritePtr()
{
	RemoveReadBuffer();
	if(writeCursor_ >= bufSize_)
	{
		return NULL;
	}
	return data +writeCursor_;
}

void Buffer::Resize(size_t size)
{
	char* oldBuf = data;
	data = new char[size];
	bufSize_ = size;
	memset(data, 0, bufSize_);
	if(0 < Size())
	{
		std::memcpy(data, oldBuf+readCursor_, Size());
	}

	writeCursor_ -= readCursor_;
	readCursor_ = 0;
	delete [] oldBuf;
}

static Pool<Buffer, std::mutex, Buffer::Init> pool_(65535);

std::shared_ptr<Buffer> Buffer::Create()
{
	return pool_.Create();
}

}


