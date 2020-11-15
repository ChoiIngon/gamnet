using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Tile : MonoBehaviour
{
	public class Data
	{
		public Message.DungeonTileType type;
		public int index;
		public Vector2Int position;
		bool _visible;

		public Data()
		{
			_visible = false;
		}

		public bool visible
		{
			get { return _visible; }
			set
			{
				_visible = value;
				OnVisible?.Invoke(_visible);
			}
		}
		public System.Action<bool> OnVisible;
	}

	public Data data;
	public SpriteRenderer spriteRenderer;
	public TouchInput touchInput;

	public void Init(Data data)
	{
		this.data = data;

		gameObject.name = "Tile_" + data.index.ToString() + "_" + data.type.ToString();
		if (Message.DungeonTileType.Floor == data.type)
		{
			touchInput = gameObject.AddComponent<TouchInput>();
			touchInput.on_touch_up = (Vector3 position) =>
			{
				Util.EventSystem.Publish<Vector2Int>(EventID.Event_OnTouch, data.position);
			};
		}
		spriteRenderer = gameObject.AddComponent<SpriteRenderer>();

		

		data.OnVisible = OnVisible;
	}

	void Start()
    {
		if (Message.DungeonTileType.Floor == data.type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/Floor/floor_vines0"); ;
		}

		if (Message.DungeonTileType.Wall == data.type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/Wall/crystal_wall00");
		}

		if (Message.DungeonTileType.Door == data.type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/dngn_open_door"); ;
		}

		if (Message.DungeonTileType.StairUp == data.type)
		{
			spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>("Tiles/Dungeon/Gateways/stone_stairs_up"); ;
		}

		if (Message.DungeonTileType.StairDown == data.type)
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
	}
}
