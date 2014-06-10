/*
 * Single.h
 *
 *  Created on: 2012. 8. 13.
 *      Author: jjaehuny
 */

#ifndef __GAMNET_LIB_SINGLETON_H_
#define __GAMNET_LIB_SINGLETON_H_

namespace Gamnet {

template <class object>
object& Singleton()
{
	static object _single;
	return _single;
}

} /*Toolkit*/

#endif /* TOOLKIT_SINGLE_H_ */
