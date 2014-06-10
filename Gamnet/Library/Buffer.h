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
        int writeCursor_;
        int readCursor_;
        int bufSize_;
        char* buf_;

        Buffer(int size = 1024) : writeCursor_(0), readCursor_(0), buf_(NULL)
        {
            bufSize_ = size;
            buf_ = new char[bufSize_];
        }

        virtual ~Buffer()
        {
            delete [] buf_;
        }

        void Append(const char* buf, int size)
        {
            if(bufSize_ < size + writeCursor_ - readCursor_)
            {
                Resize(size + writeCursor_ - readCursor_);
            }

            if(size > bufSize_ - writeCursor_)
            {
                RemoveReadBuffer();
            }

            std::memcpy(buf_+writeCursor_, buf, size);
            writeCursor_ += size;
        }

        void Remove(int size)
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

        void Resize(int size)
        {
        	char* oldBuf = buf_;
			buf_ = new char[size];
			std::memcpy(buf_, oldBuf+readCursor_, writeCursor_ - readCursor_);
			bufSize_ = size;
			writeCursor_ -= readCursor_;
			readCursor_ = 0;
			delete [] oldBuf;
        }
    protected :
        void RemoveReadBuffer()
        {
        	if(0 >= readCursor_)
        	{
        		return;
        	}
			std::memcpy(buf_, buf_+readCursor_, writeCursor_ - readCursor_);
			writeCursor_ -= readCursor_;
            readCursor_ = 0;
        }
    };
} /* namespace Toolkit */
#endif
