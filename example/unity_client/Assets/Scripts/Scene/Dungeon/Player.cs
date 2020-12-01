using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
	public SpriteRenderer trunk;
	public SpriteRenderer hair;
	public SpriteRenderer beard;
	public SpriteRenderer [] suit = new SpriteRenderer[(int)Message.EquipItemPartType.Max];
	public Unit unit;	
	// Start is called before the first frame update
	private void Start()
	{
		unit = GetComponent<Unit>();
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
		Message.Dungeon.MsgCliSvr_PlayerMove_Ntf ntf = new Message.Dungeon.MsgCliSvr_PlayerMove_Ntf();
		ntf.destination.x = to.x;
		ntf.destination.y = to.y;
		GameManager.Instance.LobbySession.SendMsg(ntf, true);
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