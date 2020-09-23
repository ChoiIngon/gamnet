#ifndef _BRESENHAM_CIRCLE_2D
#define _BRESENHAM_CIRCLE_2D

#include "Vector2.h"
#include <boost/coroutine2/all.hpp>

class BresenhamCircle2D 
{
	// https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
public :
	typedef boost::coroutines2::coroutine<Vector2Int>::pull_type Enumerator;
	typedef boost::coroutines2::coroutine<Vector2Int>::push_type Yield;
public:
	BresenhamCircle2D(const Vector2Int& center, int radious);
	Enumerator& GetEnumerator();
private :
	void Coroutine(Yield& yield);
private:
	Vector2Int center;
	int radious;
	Enumerator enumerator;
};
#endif