#ifndef _RECT_H_
#define _RECT_H_

#include "Vector2.h"

template <class T>
class TRect
{
private :
	class Max
	{
	public:
		Max(T& min, T& width);
		Max& operator = (T max);
		operator int() const;
	private:
		T& min;
		T& width;
	};
	
public:
	TRect();
	TRect(const TRect<T>& other);
	TRect(T x, T y, T width, T height);

	bool Contains(const TVector2<T>& point) const;
	bool Overlaps(const TRect<T>& other) const;
	Vector2 Center() const;

	int x;
	int y;
	int width;
	int height;

	Max xMax;
	Max yMax;

	TRect<T>& operator = (const TRect<T>& other);
};

template<class T>
TRect<T>::TRect()
	: x(0)
	, y(0)
	, width(0)
	, height(0)
	, xMax(x, width)
	, yMax(y, height)
{
}

template<class T>
TRect<T>::TRect(const TRect& other)
	: x(other.x)
	, y(other.y)
	, width(other.width)
	, height(other.height)
	, xMax(x, width)
	, yMax(y, height)
{
}

template <class T>
TRect<T>::TRect(T x, T y, T width, T height)
	: x(x)
	, y(y)
	, width(width)
	, height(height)
	, xMax(this->x, this->width)
	, yMax(this->y, this->height)
{
}

template <class T>
bool TRect<T>::Contains(const TVector2<T>& point) const
{
	return
		x <= point.x && point.x <= x + width &&
		y <= point.y && point.y <= y + height;
}

template <class T>
bool TRect<T>::Overlaps(const TRect<T>& other) const
{
	return x < other.xMax&& xMax > other.x && y < other.yMax&& yMax > other.y;
}

template <class T>
Vector2 TRect<T>::Center() const
{
	return Vector2((float)x + ((float)width / 2), (float)y + ((float)height / 2));
}

template <class T>
TRect<T>::Max::Max(T& min, T& width)
	: min(min)
	, width(width)
{
}

template <class T>
typename TRect<T>::Max& TRect<T>::Max::operator=(T max)
{
	width = max - min;
	return *this;
}

template <class T>
TRect<T>::Max::operator int() const
{
	return min + width;
}

template <class T>
TRect<T>& TRect<T>::operator = (const TRect<T>& other)
{
	x = other.x;
	y = other.y;
	width = other.width;
	height = other.height;
	return *this;
}

typedef TRect<float> Rect;
typedef TRect<int> RectInt;

#endif
