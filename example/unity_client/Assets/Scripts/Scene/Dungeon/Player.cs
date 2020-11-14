using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : Unit
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
	public override Vector2Int position
	{
		get
		{
			return new Vector2Int((int)transform.position.x, (int)transform.position.y);
		}
		set
		{
			dungeon.SetFieldOfView(this, false);
			transform.position = new Vector3(value.x, value.y, 0.0f);
			dungeon.SetFieldOfView(this, true);
		}
	}
	private void Start()
	{
		suit.cloak = SetEquipItemSprite(Message.EquipItemPartType.Cloak);
		//body.body = RandomPartsSprite(transform, "Base", "Player/Base", 1);
		//body.hair = RandomPartsSprite(transform, "Hair", "Player/Hair", 2);
		suit.shirt = SetEquipItemSprite(Message.EquipItemPartType.Body);
		suit.legs = SetEquipItemSprite(Message.EquipItemPartType.Legs);
		suit.boots = SetEquipItemSprite(Message.EquipItemPartType.Boots);
		body.beard = SetEquipItemSprite(Message.EquipItemPartType.Beard);
		suit.head = SetEquipItemSprite(Message.EquipItemPartType.Head);
		suit.gloves = SetEquipItemSprite(Message.EquipItemPartType.Gloves);
		suit.hand_1 = SetEquipItemSprite(Message.EquipItemPartType.LeftHand);
		suit.hand_2 = SetEquipItemSprite(Message.EquipItemPartType.RightHand);

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

	private GameObject SetEquipItemSprite(Message.EquipItemPartType part)
	{
		Item.Data item = GameManager.Instance.suit.GetItem(part);
		if (null == item)
		{
			return null;
		}
		GameObject obj = new GameObject();
		obj.name = part.ToString();
		obj.transform.SetParent(transform, false);
		SpriteRenderer spriteRenderer = obj.AddComponent<SpriteRenderer>();

		spriteRenderer.sprite = item.meta.equip.item_sprite;
		spriteRenderer.sortingLayerName = "Unit";
		spriteRenderer.sortingOrder = (int)part;
		return obj;
	}
}