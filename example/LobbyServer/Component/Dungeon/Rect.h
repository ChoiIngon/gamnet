#ifndef _RECT_H_
#define _RECT_H_

#include "Vector2.h"

class RectInt
{
private :
	class Max
	{
	public :
		Max(int& min, int& width);
		Max& operator = (int max);
		operator int() const;
	private :
		int& min;
		int& width;
	};
	
public :
	RectInt();
	RectInt(const RectInt& other);
	RectInt(int x, int y, int width, int height);

	bool Contains(const Vector2Int point) const;
	bool Overlaps(const RectInt& other) const;
	Vector2 Center() const;

	int x;
	int y;
	int width;
	int height;

	Max xMax;
	Max yMax;

	RectInt& operator = (const RectInt& other);
};


#endif
