using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : Unit
{
	public SpriteRenderer trunk;
	public SpriteRenderer hair;
	public SpriteRenderer beard;
	public SpriteRenderer [] suit = new SpriteRenderer[(int)Message.EquipItemPartType.Max];
	
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
		Util.EventSystem.Subscribe<Vector2Int>(EventID.Event_OnTouch, OnMove);
		Util.EventSystem.Subscribe<Item.Data>(Component.Suit.Event.EquipItem, OnEquip);
		Util.EventSystem.Subscribe<Item.Data>(Component.Suit.Event.UnequipItem, OnUnequip);

		for (Message.EquipItemPartType part = Message.EquipItemPartType.Invalid + 1; part < Message.EquipItemPartType.Max; part++)
		{
			Item.Data item = GameManager.Instance.suit.GetItem(part);
			if (null == item)
			{
				OnUnequip(part);
			}
			else
			{
				OnEquip(item);
			}
		}

		trunk.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(Component.Suit.DEFAULT_BASE_IMAGE);
		hair.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(Component.Suit.DEFAULT_HAIR_IMAGE);
	}

	private void OnDestroy()
	{
		Util.EventSystem.Unsubscribe<Vector2Int>(EventID.Event_OnTouch);
		Util.EventSystem.Unsubscribe<Item.Data>(Component.Suit.Event.EquipItem, OnEquip);
		Util.EventSystem.Unsubscribe<Item.Data>(Component.Suit.Event.UnequipItem, OnUnequip);
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

	public void OnEquip(Item.Data item)
	{
		OnEquip(item.meta.equip.part, item.meta.equip.item_sprite);
	}

	public void OnEquip(Message.EquipItemPartType part, Sprite sprite)
	{
		SpriteRenderer spriteRenderer = suit[(int)part];
		spriteRenderer.sprite = sprite; 
		spriteRenderer.gameObject.SetActive(true);
	}

	public void OnUnequip(Item.Data item)
	{
		OnUnequip(item.meta.equip.part);
	}

	public void OnUnequip(Message.EquipItemPartType part)
	{
		suit[(int)part].gameObject.SetActive(false);
	}
}