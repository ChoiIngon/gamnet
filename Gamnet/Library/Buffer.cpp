
/*
 * Buffer.cpp
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#include "Buffer.h"
#include "Pool.h"

namespace Gamnet {

//int Buffer::MAX_SIZE = 0x0000ffff;

void Buffer::Swap(const std::shared_ptr<Buffer>& lhs, const std::shared_ptr<Buffer>& rhs)
{
	char* tmp_data = lhs->data;
	size_t tmp_write = lhs->write_index;
	size_t tmp_read = lhs->read_index;
	size_t tmp_size = lhs->bufSize_;

	lhs->data = rhs->data;
	lhs->write_index = rhs->write_index;
	lhs->read_index = rhs->read_index;
	lhs->bufSize_ = rhs->bufSize_;

	rhs->data = tmp_data;
	rhs->write_index = tmp_write;
	rhs->read_index = tmp_read;
	rhs->bufSize_ = tmp_size;
}

Buffer::Buffer(size_t size) : bufSize_(size), data(nullptr), write_index(0), read_index(0)
{
	data = new char[bufSize_];
	if(nullptr == data)
	{
		throw Exception(ENOMEM, "not enough space(need:", size," bytes)");
	}
	memset(data, 0, bufSize_);
}

Buffer::~Buffer()
{
	if(nullptr != data)
	{
		delete [] data;
		data = nullptr;
	}
}

size_t Buffer::Available() const
{
	return bufSize_ - Size();
}

size_t Buffer::Capacity() const
{
	return bufSize_;
}

void Buffer::Clear()
{
	write_index = 0;
	read_index = 0;
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

	if(size > bufSize_ - write_index)
	{
		RemoveReadBuffer();
	}

	std::memcpy(WritePtr(), buf, size);
	write_index += size;
}

void Buffer::Copy(const Buffer& buffer)
{
	read_index = 0;
	write_index = 0;
	if(Available() < buffer.Size())
	{
		Resize(buffer.Size());
	}
	std::memcpy(data, buffer.ReadPtr(), buffer.Size());
	write_index += buffer.Size();
}

void Buffer::Remove(uint16_t size)
{
	read_index += size;
	assert(read_index <= write_index);
	if(read_index > write_index)
	{
		read_index = write_index;
	}
	if(read_index == write_index)
	{
		write_index = 0;
		read_index = 0;
	}
}

void Buffer::RemoveReadBuffer()
{
	if (0 >= read_index)
	{
		return;
	}
	if (0 < Size())
	{
		std::memcpy(data, ReadPtr(), Size());
	}
	write_index -= read_index;
	read_index = 0;
}

size_t Buffer::Size() const
{
	return write_index - read_index;
}

const char* Buffer::ReadPtr() const
{
	return data + read_index;
}

char* Buffer::WritePtr()
{
	/*
	RemoveReadBuffer();
	if(write_index >= bufSize_)
	{
		return NULL;
	}
	*/
	return data + write_index;
}

void Buffer::Resize(size_t size)
{
	char* oldBuf = data;
	char* newBuf = new char[size];
	if (0 < Size())
	{
		std::memcpy(newBuf, ReadPtr(), Size());
	}

	write_index -= read_index;
	read_index = 0;

	data = newBuf;
	bufSize_ = size;
	
	delete [] oldBuf;
}

static Pool<Buffer, std::mutex, Buffer::Init> pool_(65535);

std::shared_ptr<Buffer> Buffer::Create()
{
	return pool_.Create();
}

}


