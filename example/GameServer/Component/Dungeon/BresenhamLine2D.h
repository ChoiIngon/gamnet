#ifndef _BRESENHAM_LINE_2D
#define _BRESENHAM_LINE_2D

#include "Vector2.h"
#include <boost/coroutine2/all.hpp>

class BresenhamLine2D
{
	// https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
public:
	typedef boost::coroutines2::coroutine<Vector2Int>::pull_type Enumerator;
	typedef boost::coroutines2::coroutine<Vector2Int>::push_type Yield;
public:
	BresenhamLine2D(const Vector2Int& start, const Vector2Int& end);
	Enumerator& GetEnumerator();

	BresenhamLine2D(BresenhamLine2D const & other) = delete;

	BresenhamLine2D(BresenhamLine2D && other) noexcept;

	BresenhamLine2D& operator=(BresenhamLine2D const&) = delete;
	BresenhamLine2D& operator=(BresenhamLine2D&& other) noexcept;
	explicit operator bool() const noexcept;
	bool operator!() const noexcept;

	Vector2Int operator()();
private:
	void Coroutine(Yield& yield);
private:
	Vector2Int start;
	Vector2Int end;
	Enumerator enumerator;
};
#endif