using UnityEngine;
using System.Collections.Generic;
using System;

public class Unit : MonoBehaviour
{
	public UInt64 seq;
	public int max_health;
	public int cur_health;
	public int attack;
	public int defense;
	public int speed;
	public int sight;
	public float critical_chance;
	public float critical_rate;

	public Vector2Int position
	{
		get
		{
			return new Vector2Int((int)transform.position.x, (int)transform.position.y);
		}
		set
		{
			Player player = GetComponent<Player>();
			if (null != player)
			{
				player.SetFieldOfView(value, false);
			}

			transform.position = new Vector3(value.x, value.y, 0.0f);

			if (null != player)
			{
				player.SetFieldOfView(value, true);
			}
		}
	}

	public System.Action<float> onDamage;

	public void SetMovePath(List<Vector2Int> path)
	{
		this.path = path;
		this.delta_time = 1.0f;
	}

	private List<Vector2Int> path = null;
	private float delta_time = 0.0f;

	private void Update()
	{
		if (null != path)
		{
			if (0 >= path.Count)
			{
				path = null;
				return;
			}

			delta_time += Time.deltaTime;
			if (0.1f > delta_time)
			{
				return;
			}

			delta_time = 0.0f;
			position = path[0];
			path.RemoveAt(0);
		}
	}

	static public GameObject InitPartsSprite(Transform parent, string name, string spritePath, int sortingOrder)
	{
		GameObject obj = new GameObject();
		obj.name = name;
		obj.transform.SetParent(parent, false);
		SpriteRenderer spriteRenderer = obj.AddComponent<SpriteRenderer>();
		spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>(spritePath);
		spriteRenderer.sortingLayerName = "Unit";
		spriteRenderer.sortingOrder = sortingOrder;
		return obj;
	}
}
