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
        int32_t writeCursor_;
        int32_t readCursor_;
        uint16_t bufSize_;
        char* buf_;

        Buffer(uint16_t size = 1024);
        virtual ~Buffer();

        void Append(const char* buf, uint16_t size);
        /**
         * remove read byte, actually just move read pointer
         */
        void Remove(uint16_t size);

        /**
         * return size of bytes which is now using
         */
        size_t Size() const;

        const char* ReadPtr() const;
        char* WritePtr();

        /**
         * return : return available buffer size
         */
        size_t Available() const;
        /**
		 * return : return maximum available buffer size
		 */
        size_t Capacity() const;

        /**
         * set zero write pointer cursor & read pointer cursor
         */
        void Clear();

        void Resize(uint16_t size);
        void Copy(const Buffer& buffer);
    protected :
        void RemoveReadBuffer();
    };
} /* namespace Toolkit */
#endif
