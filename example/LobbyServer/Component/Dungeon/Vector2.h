#ifndef _VECTOR2_H_
#define _VECTOR2_H_
#include <math.h>
#include <algorithm>

template <class T>
class TVector2
{
public :
	TVector2()
	{
		x = {};
		y = {};
	}

	TVector2(T x, T y)
		: x(x)
		, y(y)
	{
	}

	static TVector2<T> zero;
	static TVector2<T> Normalize(const TVector2<T>& vec)
	{
		float length = abs(sqrt(vec.x * vec.x + vec.y * vec.y));
		if (0.0000001f > length)
		{
			length = 0.0f;
		}
		TVector2<T> ret = vec;
		ret.x = vec.x / length;
		ret.y = vec.y / length;
		return ret;
	}

	static float Distance(const TVector2<T>& lhs, const TVector2<T>& rhs)
	{
		return sqrtf(powf((float)(lhs.x - rhs.x), 2.0f) + powf((float)(lhs.y - rhs.y), 2.0f));
	}

	T x;
	T y;
};

template <class T>
TVector2<T> TVector2<T>::zero = TVector2<T>(0, 0);

template <class T>
bool operator == (const TVector2<T>& lhs, const TVector2<T>& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <class T>
bool operator != (const TVector2<T>& lhs, const TVector2<T>& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y;
}

typedef TVector2<float> Vector2;
typedef TVector2<int> Vector2Int;

#endif
