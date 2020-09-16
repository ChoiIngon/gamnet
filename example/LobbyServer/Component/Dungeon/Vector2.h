#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#include <idl/MessageCommon.h>

struct Vector2
{
	Vector2();
	Vector2(float x, float y);

	float x;
	float y;
};

struct Vector2Int
{
	Vector2Int();
	Vector2Int(int x, int y);
	int x;
	int y;

	Vector2Int operator = (const Message::Vector2Int& rhs);
	operator Message::Vector2Int () const;
};

#endif
