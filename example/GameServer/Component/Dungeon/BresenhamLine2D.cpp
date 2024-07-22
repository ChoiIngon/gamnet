#include "BresenhamLine2D.h"


BresenhamLine2D::BresenhamLine2D(const Vector2Int& start, const Vector2Int& end)
	: start(start)
	, end(end)
	, enumerator(std::bind(&BresenhamLine2D::Coroutine, this, std::placeholders::_1))
{
}

BresenhamLine2D::BresenhamLine2D(BresenhamLine2D && other) noexcept
	: start(other.start)
	, end(other.end)
	, enumerator(std::move(other.enumerator))
{
}

BresenhamLine2D& BresenhamLine2D::operator=(BresenhamLine2D&& other) noexcept
{
	start = other.start;
	end = other.end;
	std::swap(enumerator, other.enumerator);
	return *this;
}

BresenhamLine2D::operator bool() const noexcept
{
	return (bool)enumerator;
}

bool BresenhamLine2D::operator!() const noexcept
{
	return !(bool)enumerator;
}

BresenhamLine2D::Enumerator& BresenhamLine2D::GetEnumerator()
{
	return enumerator;
}

Vector2Int BresenhamLine2D::operator()()
{
	Vector2Int vector = enumerator.get();
	enumerator();
	return vector;
}

void BresenhamLine2D::Coroutine(Yield& yield)
{
	int dx = std::abs(end.x - start.x); // 시작 점과 끝 점의 각 x 좌표의 거리
	int dy = std::abs(end.y - start.y); // 시작 점과 끝 점의 각 y 좌표의 거리

	if (dy <= dx)
	{
		int p = 2 * (dy - dx);
		int y = start.y;

		int inc_x = 1;
		if (end.x < start.x)
		{
			inc_x = -1;
		}

		int inc_y = 1;
		if (end.y < start.y)
		{
			inc_y = -1;
		}

		for (int x = start.x; (start.x <= end.x ? x <= end.x : x >= end.x); x += inc_x)
		{
			if (0 >= p)
			{
				p += 2 * dy;
			}
			else
			{
				p += 2 * (dy - dx);
				y += inc_y;
			}
			yield(Vector2Int(x, y));
		}
	}
	else
	{
		int p = 2 * (dx - dy);
		int x = start.x;

		int inc_x = 1;
		if (end.x < start.x)
		{
			inc_x = -1;
		}
		int inc_y = 1;
		if (end.y < start.y)
		{
			inc_y = -1;
		}
		for (int y = start.y; (start.y <= end.y ? y <= end.y : y >= end.y); y += inc_y)
		{
			if (0 >= p)
			{
				p += 2 * dx;
			}
			else
			{
				p += 2 * (dx - dy);
				x += inc_x;
			}
			yield(Vector2Int(x, y));
		}
	}
}