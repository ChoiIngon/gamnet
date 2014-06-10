/*
 * String.h
 *
 *  Created on: Jul 8, 2013
 *      Author: kukuta
 */

#ifndef __GAMNET_LIB_STRING_H_
#define __GAMNET_LIB_STRING_H_

#include <string>
#include <sstream>

namespace Gamnet {

class String
{
private :
	std::stringstream stream_;
    template <class T>
    void Concat(const T& t)
    {
        stream_ << t;
    }

    template <class T, class... ARGS>
    void Concat(const T& t, ARGS&&... args)
    {
        stream_ << t;
        Concat(args...);
    }
public :
    template <class... ARGS>
	String(ARGS... args)
	{
		Concat(args...);
	}

    const char* c_str() const
    {
    	return stream_.str().c_str();
    }
};

};



#endif /* STRING_H_ */
