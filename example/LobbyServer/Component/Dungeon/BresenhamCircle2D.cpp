#include "BresenhamCircle2D.h"


BresenhamCircle2D::BresenhamCircle2D(const Vector2Int& center, int radious)
	: center(center)
	, radious(radious)
	, Gamnet::Coroutine<Vector2Int>(std::bind(&BresenhamCircle2D::Coroutine, this, std::placeholders::_1))
{
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