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

template <class T>
void Concat(std::stringstream& stream, const T& t)
{
	stream << t;
}

template <class T, class... ARGS>
void Concat(std::stringstream& stream, const T& t, ARGS&&... args)
{
	stream << t;
	Concat(stream, args...);
}

template <class... ARGS>
std::string Concat(ARGS... args)
{
	std::stringstream stream;
	Concat(stream, args...);
	return stream.str();
}

template <class... ARGS>
std::string Format(ARGS... args)
{
	return Concat(args...);
}
};



#endif /* STRING_H_ */
