#include "Vector2.h"

Vector2::Vector2()
	: x(0.0f)
	, y(0.0f)
{
}

Vector2::Vector2(float x, float y)
	: x(x)
	, y(y)
{
}

Vector2Int::Vector2Int()
	: x(0)
	, y(0)
{
}

Vector2Int::Vector2Int(int x, int y) 
	: x(x)
	, y(y)
{
}

Vector2Int Vector2Int::operator = (const Message::Vector2Int& rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

Vector2Int::operator Message::Vector2Int() const
{
	Message::Vector2Int vec;
	vec.x = x;
	vec.y = y;
	return vec;
}