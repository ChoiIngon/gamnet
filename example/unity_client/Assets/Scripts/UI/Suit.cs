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
		Util.EventSystem.Subscribe<Item.Data>(Component.Suit.Event.UnequipItem, OnUnequip);

		for (Message.EquipItemPartType part = Message.EquipItemPartType.Invalid + 1; part < Message.EquipItemPartType.Max; part++)
		{
			Item.Data item = GameManager.Instance.suit.GetItem(part);
			if (null == item)
			{
				item_images[(int)part].gameObject.SetActive(false);
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
		Util.EventSystem.Unsubscribe<Item.Data>(Component.Suit.Event.UnequipItem, OnUnequip);
	}

	public void OnEquip(Item.Data item)
    {
		Image image = item_images[(int)item.meta.equip.part];
		image.sprite = item.meta.equip.item_sprite; //AssetBundleManager.Instance.LoadAsset<Sprite>(item.meta.equip.sprite_path);
		image.gameObject.SetActive(true);
    }

    public void OnUnequip(Item.Data item)
    {
		item_images[(int)item.meta.equip.part].gameObject.SetActive(false);
    }
}
