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

	BresenhamCircle2D(BresenhamCircle2D const& other) = delete;

	BresenhamCircle2D(BresenhamCircle2D&& other) noexcept;

	BresenhamCircle2D& operator=(BresenhamCircle2D const&) = delete;
	BresenhamCircle2D& operator=(BresenhamCircle2D&& other) noexcept;
	explicit operator bool() const noexcept;
	bool operator!() const noexcept;

	Vector2Int operator()();
private :
	void Coroutine(Yield& yield);
private:
	Vector2Int center;
	int radious;
	Enumerator enumerator;
};
#endif