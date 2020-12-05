using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
	public Camera camera;
	public Unit unit;
	private void Awake()
	{
		unit = GetComponent<Unit>();
	}
	private void Start()
	{
		Util.EventSystem.Subscribe<Vector2Int>(EventID.Event_OnTouch, OnMove);
		Util.EventSystem.Subscribe<Item.Data>(Component.Suit.Event.EquipItem, OnEquip);
		Util.EventSystem.Subscribe<Item.Data>(Component.Suit.Event.UnequipItem, OnUnequip);

		Player player = GetComponent<Player>();
		for (Message.EquipItemPartType part = Message.EquipItemPartType.Invalid + 1; part < Message.EquipItemPartType.Max; part++)
		{
			Item.Data item = GameManager.Instance.suit.GetItem(part);
			if (null == item)
			{
				player.OnUnequip(part);
			}
			else
			{
				player.OnEquip(item.meta.equip.part, item.meta.equip.item_sprite);
			}
		}
	}

	private void OnEnable()
	{
		camera.enabled = true;
	}
	private void OnDisable()
	{
		camera.enabled = false;
	}
	private void OnDestroy()
	{
		Util.EventSystem.Unsubscribe<Vector2Int>(EventID.Event_OnTouch, OnMove);
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
		Player player = GetComponent<Player>();
		player.OnEquip(item.meta.equip.part, item.meta.equip.item_sprite);
	}

	public void OnUnequip(Item.Data item)
	{
		Player player = GetComponent<Player>();
		player.OnUnequip(item.meta.equip.part);
	}
}

