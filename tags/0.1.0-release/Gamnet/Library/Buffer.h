#ifndef __GAMNET_LIB_BUFFER_H__
#define __GAMNET_LIB_BUFFER_H__
/*
 * Buffer.cpp
 *
 * Created on: Jul 4, 2013
 * Author: kukuta
 */

#include <vector>
#include <cstring>
#include "Exception.h"
#ifdef _DEBUG
#include <assert.h>
#endif

namespace Gamnet {

    struct Buffer
    {
    	struct Init
		{
    		Buffer* operator() (Buffer* buffer)
			{
				buffer->Clear();
				return buffer;
			}
		};
        uint16_t writeCursor_;
        uint16_t readCursor_;
        uint16_t bufSize_;
        char* buf_;

        Buffer(uint16_t size = 1024) : writeCursor_(0), readCursor_(0), bufSize_(size), buf_(NULL)
        {
            buf_ = new char[bufSize_];
#ifdef _DEBUG
            assert(NULL != buf_);
#endif
            memset(buf_, 0, bufSize_);
        }
        virtual ~Buffer()
        {
            delete [] buf_;
        }

        void Append(const char* buf, uint16_t size)
        {
        	/*
            if(bufSize_ < size + writeCursor_ - readCursor_)
            {
                Resize(size + writeCursor_ - readCursor_);
            }
            */
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

        void Remove(uint16_t size)
        {
            (readCursor_ += size) > writeCursor_ ? readCursor_ = writeCursor_ : readCursor_;
        }

        size_t Size() const
        {
            return writeCursor_ - readCursor_;
        }

        const char* ReadPtr() const
        {
            return buf_+readCursor_;
        }
        char* WritePtr()
        {
			RemoveReadBuffer();
			if(writeCursor_ >= bufSize_)
			{
				return NULL;
			}
            return buf_+writeCursor_;
        }

        size_t Available() const
        {
            return bufSize_ - (writeCursor_ - readCursor_);
        }

        size_t Capacity() const
        {
            return bufSize_;
        }

        void Clear()
        {
            writeCursor_ = 0;
            readCursor_ = 0;
        }

        void Resize(uint16_t size)
        {
        	char* oldBuf = buf_;
        	buf_ = new char[size];
#ifdef _DEBUG
			assert(NULL != buf_);
#endif
        	bufSize_ = size;
			memset(buf_, 0, bufSize_);
			if(0 < writeCursor_-readCursor_)
			{
				std::memcpy(buf_, oldBuf+readCursor_, writeCursor_ - readCursor_);
			}

			writeCursor_ -= readCursor_;
			readCursor_ = 0;
			delete [] oldBuf;
        }
        void Copy(const Buffer& buffer);
    protected :
        void RemoveReadBuffer()
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
    };
} /* namespace Toolkit */
#endif
