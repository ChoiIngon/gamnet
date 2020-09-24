#include "BresenhamCircle2D.h"


BresenhamCircle2D::BresenhamCircle2D(const Vector2Int& center, int radious)
	: center(center)
	, radious(radious)
	, enumerator(std::bind(&BresenhamCircle2D::Coroutine, this, std::placeholders::_1))
{
}

BresenhamCircle2D::BresenhamCircle2D(BresenhamCircle2D&& other) noexcept
	: center(other.center)
	, radious(other.radious)
	, enumerator(std::move(other.enumerator))
{
}

BresenhamCircle2D& BresenhamCircle2D::operator=(BresenhamCircle2D&& other) noexcept
{
	center = other.center;
	radious = other.radious;
	std::swap(enumerator, other.enumerator);
	return *this;
}

BresenhamCircle2D::operator bool() const noexcept
{
	return (bool)enumerator;
}

bool BresenhamCircle2D::operator!() const noexcept
{
	return !(bool)enumerator;
}

BresenhamCircle2D::Enumerator& BresenhamCircle2D::GetEnumerator()
{
	return enumerator;
}

Vector2Int BresenhamCircle2D::operator()()
{
	Vector2Int vector = enumerator.get();
	enumerator();
	return vector;
}

void BresenhamCircle2D::Coroutine(Yield& yield)
{
	if (0 >= radious)
	{
		return;
	}

	int xK = 0;
	int yK = radious;
	int pK = 3 - (radious + radious);

	do
	{
		yield(Vector2Int(center.x + xK, center.y - yK));
		yield(Vector2Int(center.x - xK, center.y - yK));
		yield(Vector2Int(center.x + xK, center.y + yK));
		yield(Vector2Int(center.x - xK, center.y + yK));
		yield(Vector2Int(center.x + yK, center.y - xK));
		yield(Vector2Int(center.x - yK, center.y - xK));
		yield(Vector2Int(center.x + yK, center.y + xK));
		yield(Vector2Int(center.x - yK, center.y + xK));

		xK++;
		if (pK < 0)
		{
			pK += (xK << 2) + 6;
		}
		else
		{
			--yK;
			pK += ((xK - yK) << 2) + 10;
		}
	} while (xK <= yK);
}