#ifndef _BRESENHAM_CIRCLE_2D
#define _BRESENHAM_CIRCLE_2D

#include "Vector2.h"
#include <Gamnet/Library/Coroutine.h>

class BresenhamCircle2D : public Gamnet::Coroutine<Vector2Int>
{
	// https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
public:
	BresenhamCircle2D(const Vector2Int& center, int radious);
private :
	void Coroutine(Yield& yield);
private:
	Vector2Int center;
	int radious;
};
#endif