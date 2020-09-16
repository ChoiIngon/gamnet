using UnityEngine;
using System.Collections;

public class BresenhamCircle2D : IEnumerable
{
	private Vector2Int center;
	private int radious;

	public BresenhamCircle2D(Vector2Int center, int radious)
	{
		this.center = center;
		this.radious = radious;
	}

	public IEnumerator GetEnumerator()
	{
		if (0 >= radious)
		{
			yield break;
		}

		int xK = 0;
		int yK = radious;
		int pK = 3 - (radious + radious);

        do
		{
			yield return new Vector2Int(center.x + xK, center.y - yK);
			yield return new Vector2Int(center.x - xK, center.y - yK);
			yield return new Vector2Int(center.x + xK, center.y + yK);
			yield return new Vector2Int(center.x - xK, center.y + yK);
			yield return new Vector2Int(center.x + yK, center.y - xK);
			yield return new Vector2Int(center.x - yK, center.y - xK);
			yield return new Vector2Int(center.x + yK, center.y + xK);
			yield return new Vector2Int(center.x - yK, center.y + xK);

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
}