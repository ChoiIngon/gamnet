using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Suit : MonoBehaviour
{
    public Image[] item_images = new Image[(int)Message.EquipItemPartType.Max];

	private void OnEnable()
	{
		Util.EventSystem.Subscribe<Item.Data>(Component.Suit.Event.EquipItem, OnEquip);
		Util.EventSystem.Subscribe<Message.EquipItemPartType>(Component.Suit.Event.UnequipItem, OnUnequip);

		for (Message.EquipItemPartType part = Message.EquipItemPartType.Invalid; part < Message.EquipItemPartType.Max; part++)
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
	}

	private void OnDisable()
	{
		Util.EventSystem.Unsubscribe<Item.Data>(Component.Suit.Event.EquipItem, OnEquip);
		Util.EventSystem.Unsubscribe<Message.EquipItemPartType>(Component.Suit.Event.UnequipItem, OnUnequip);
	}

	public void OnEquip(Item.Data item)
    {
		Image image = item_images[(int)item.meta.equip.part];
		image.color = Color.white;
		image.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(item.meta.equip.sprite_path);
    }

    public void OnUnequip(Message.EquipItemPartType part)
    {
		if (Message.EquipItemPartType.Invalid == part)
		{
			return;
		}
		item_images[(int)part].color = new Color32(0, 0, 0, 0);
    }
}
