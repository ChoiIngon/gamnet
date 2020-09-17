using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
	public Vector2Int position
	{
		get { return new Vector2Int((int)transform.position.x, (int)transform.position.y); }
		set
		{
			SetFieldOfView(this.position, false);
			transform.position = new Vector3(value.x, value.y, 0.0f);
			SetFieldOfView(value, true);
			//Camera.main.transform.position = new Vector3(position.x, position.y, Camera.main.transform.position.z);
		}
	}
	public Unit unit = new Unit();
	private GameObject self;
	private GameObject body;
	private GameObject boots;
	private GameObject cloak;
	private GameObject gloves;
	private GameObject hair;
	private GameObject hand_1;
	private GameObject hand_2;
	private GameObject head;
	private GameObject legs;
	// Start is called before the first frame update
	void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

	private void SetFieldOfView(Vector2Int position, bool visible)
	{
		Component.Dungeon dungeon = GameManager.Instance.scenes.dungeon_main.dungeon;
		BresenhamCircle2D circle = new BresenhamCircle2D(this.position, unit.sight - 1);
		foreach (Vector2Int circumference in circle)
		{
			BresenhamLine2D line = new BresenhamLine2D(this.position, circumference);
			foreach (Vector2Int point in line)
			{
				Tile.Data tile = dungeon.GetTileData(point.x, point.y);
				tile.visible = visible;

				if (Message.DungeonTileType.Wall == tile.type)
				{
					break;
				}

				if (dungeon.width * dungeon.height > tile.index + 1)
				{
					dungeon.GetTileData(point.x + 1, point.y).visible = visible;
				}

				if (dungeon.width * dungeon.height > tile.index + dungeon.width)
				{
					dungeon.GetTileData(point.x, point.y + 1).visible = visible;
				}

				if (0 <= tile.index - 1)
				{
					dungeon.GetTileData(point.x - 1, point.y).visible = visible;
				}

				if (0 <= tile.index - dungeon.width)
				{
					dungeon.GetTileData(point.x, point.y - 1).visible = visible;
				}
			}
		}
	}
}
