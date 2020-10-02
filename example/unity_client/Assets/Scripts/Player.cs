﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
	public class Suit
	{
		public GameObject head;
		public GameObject shirt;
		public GameObject cloak;
		public GameObject gloves;
		public GameObject hand_1;
		public GameObject hand_2;
		public GameObject legs;
		public GameObject boots;
	}

	public class Body
	{
		public GameObject body;
		public GameObject hair;
		public GameObject beard;
	}

	public Body body = new Body();
	public Suit suit = new Suit();

	// Start is called before the first frame update
	private void Start()
    {
		body.body = RandomPartsSprite(transform, "Body", "Player/Base", 1);
		body.beard = RandomPartsSprite(transform, "Beard", "Player/Beard", 3);
		body.hair = RandomPartsSprite(transform, "Hair", "Player/Hair", 2);
		suit.cloak = RandomPartsSprite(transform, "Clock", "Player/Cloak", 0);
		suit.shirt = RandomPartsSprite(transform, "Shirt", "Player/Body", 2);
		suit.gloves = RandomPartsSprite(transform, "Gloves", "Player/Gloves", 3);
		suit.hand_1 = RandomPartsSprite(transform, "Hand1", "Player/Hand1", 4);
		suit.hand_2 = RandomPartsSprite(transform, "Hand2", "Player/Hand2", 4);
		suit.head = RandomPartsSprite(transform, "Head", "Player/Head", 3);
		suit.legs = RandomPartsSprite(transform, "Legs", "Player/Legs", 2);
		suit.boots = RandomPartsSprite(transform, "Boots", "Player/Boots", 2);

		Util.EventSystem.Subscribe<Vector2Int>(EventID.Event_OnTouch, OnMove);
	}

	private void OnDestroy()
	{
		Util.EventSystem.Unsubscribe<Vector2Int>(EventID.Event_OnTouch);
	}

	private void OnMove(Vector2Int to)
	{
		/*
		AStarPathFinder pathFinder = new AStarPathFinder(GameManager.Instance.scenes.dungeon_main.dungeon, position, to);
		if (0 == pathFinder.path.Count)
		{
			return;
		}

		pathFinder.path.RemoveAt(0);
		path = pathFinder.path;
		*/
		Handler.Dungeon.Handler_PlayerMove.SendMsg(to);
	}
	
	public void SetFieldOfView(Vector2Int position, bool visible)
	{
		Unit self = GetComponent<Unit>();
		Component.Dungeon dungeon = GameManager.Instance.scenes.dungeon_main.dungeon;
		BresenhamCircle2D circle = new BresenhamCircle2D(self.position, 10 - 1);

		foreach (Vector2Int circumference in circle)
		{
			BresenhamLine2D line = new BresenhamLine2D(self.position, circumference);
			foreach (Vector2Int point in line)
			{
				Tile.Data tile = dungeon.GetTileData(point.x, point.y);
				if (null == tile)
				{
					break;
				}
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

	private GameObject RandomPartsSprite(Transform parent, string name, string spriteDirPath, int sortingOrder)
	{
		GameObject obj = new GameObject();
		obj.name = name;
		obj.transform.SetParent(parent, false);
		SpriteRenderer spriteRenderer = obj.AddComponent<SpriteRenderer>();
		Sprite[] sprites = Resources.LoadAll<Sprite>(spriteDirPath);
		spriteRenderer.sprite = sprites[Random.Range(0, sprites.Length)];
		spriteRenderer.sortingLayerName = "Unit";
		spriteRenderer.sortingOrder = sortingOrder;
		return obj;
	}
}
