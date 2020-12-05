using System;
using System.Collections.Generic;
using UnityEngine;

public class Tile : MonoBehaviour
{
	public int index;
	public Message.DungeonTileType type;
	public Vector2Int position;
	public Unit unit;
	private bool _visible = false;

	public bool visible
	{
		get { return _visible; }
		set
		{
			_visible = value;
			OnVisible(value);
		}
	}

	public SpriteRenderer spriteRenderer;
	public TouchInput touchInput;

	public void Init(int index, Message.DungeonTileType type, Vector2Int position)
	{
		this.index = index;
		this.type = type;
		this.position = position;

		gameObject.name = "Tile_" + index.ToString() + "_" + type.ToString();
		
		if (Message.DungeonTileType.Floor == type)
		{
			touchInput = gameObject.AddComponent<TouchInput>();
			touchInput.on_touch_up = (Vector3 pos) =>
			{
				Util.EventSystem.Publish<Vector2Int>(EventID.Event_OnTouch, this.position);
			};
		}
		spriteRenderer = gameObject.AddComponent<SpriteRenderer>();
	}

	void Start()
    {
		if (Message.DungeonTileType.Floor == type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/Floor/rect_gray" + UnityEngine.Random.Range(0, 4).ToString());
		}

		if (Message.DungeonTileType.Wall == type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/Wall/brick_brown" + UnityEngine.Random.Range(0, 8).ToString());
		}

		if (Message.DungeonTileType.Door == type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/dngn_open_door"); ;
		}

		if (Message.DungeonTileType.StairUp == type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/Gateways/stone_stairs_up"); ;
		}

		if (Message.DungeonTileType.StairDown == type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/Gateways/stone_stairs_down"); ;
		}
	}

	public void OnVisible(bool flag)
	{
		if (true == flag)
		{
			gameObject.SetActive(true);
			spriteRenderer.color = Color.white;
		}
		else
		{
			spriteRenderer.color = new Color(0.5f, 0.5f, 0.5f);
		}
		if (null != unit)
		{
			unit.gameObject.SetActive(flag);
		}
	}

	public void Enter(Unit unit)
	{
		this.unit = unit;
		unit.gameObject.SetActive(visible);
	}
	public void Leave(Unit unit)
	{
		if (this.unit == unit)
		{
			this.unit = null;
		}
	}
}
