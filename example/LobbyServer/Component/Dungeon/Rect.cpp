#include "Rect.h"

RectInt::RectInt()
	: x(0)
	, y(0)
	, width(0)
	, height(0)
	, xMax(x, width)
	, yMax(y, height)
{
}

RectInt::RectInt(const RectInt& other)
	: x(other.x)
	, y(other.y)
	, width(other.width)
	, height(other.height)
	, xMax(x, width)
	, yMax(y, height)
{
}

RectInt::RectInt(int x, int y, int width, int height)
	: x(x)
	, y(y)
	, width(width)
	, height(height)
	, xMax(this->x, this->width)
	, yMax(this->y, this->height)
{
}

bool RectInt::Contains(const Vector2Int point) const
{
	return
		x <= point.x && point.x <= x + width &&
		y <= point.y && point.y <= y + height;
}

bool RectInt::Overlaps(const RectInt& other) const
{
	return x < other.xMax && xMax > other.x && y < other.yMax && yMax > other.y;
}

Vector2 RectInt::Center() const
{
	return Vector2((float)x + ((float)width / 2), (float)y + ((float)height / 2));
}

RectInt::Max::Max(int& min, int& width)
	: min(min)
	, width(width)
{
}

RectInt::Max& RectInt::Max::operator=(int max)
{
	width = max - min;
	return *this;
}

RectInt::Max::operator int() const
{
	return min + width;
}

RectInt& RectInt::operator = (const RectInt& other)
{
	x = other.x;
	y = other.y;
	width = other.width;
	height = other.height;
	return *this;
}
