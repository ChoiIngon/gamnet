#include "Vector2.h"
#include <math.h>

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

Vector2Int Vector2Int::zero = Vector2Int(0, 0);

/*
Vector2Int Normalize(const Vector2Int& vec)
{
	float length = abs(sqrt(vec.x * vec.x + vec.y * vec.y));
	if (0.0000001f > length)
	{
		length = 0.0f;
	}
	Vector2Int ret = vec;
	ret.x = vec.x / length;
	ret.y = vec.y / length;
	return ret;
}
*/

float Vector2Int::Distance(const Vector2Int& lhs, const Vector2Int& rhs)
{
	return sqrt(pow(lhs.x - rhs.x, 2.0f) + pow(lhs.y - rhs.y, 2.0f));
}

bool operator == (const Vector2Int& lhs, const Vector2Int& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator != (const Vector2Int& lhs, const Vector2Int& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y;
}