#ifndef _VECTOR2_H_
#define _VECTOR2_H_

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
};

#endif
