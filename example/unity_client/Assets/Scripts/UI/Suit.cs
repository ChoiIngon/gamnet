using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Suit : MonoBehaviour
{
	public Image trunk_image;
	public Image hair_image;
	public Image[] suit_images = new Image[(int)Message.EquipItemPartType.Max];
	
	private void OnEnable()
	{
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

		trunk_image.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(Component.Suit.DEFAULT_BASE_IMAGE);
		hair_image.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(Component.Suit.DEFAULT_HAIR_IMAGE);
	}

	private void OnDisable()
	{
		Util.EventSystem.Unsubscribe<Item.Data>(Component.Suit.Event.EquipItem, OnEquip);
		Util.EventSystem.Unsubscribe<Item.Data>(Component.Suit.Event.UnequipItem, OnUnequip);
	}

	public void OnEquip(Item.Data item)
    {
		OnEquip(item.meta.equip.part, item.meta.equip.item_sprite);
    }

	public void OnEquip(Message.EquipItemPartType part, Sprite sprite)
	{
		Image image = suit_images[(int)part];
		image.sprite = sprite; //AssetBundleManager.Instance.LoadAsset<Sprite>(item.meta.equip.sprite_path);
		image.gameObject.SetActive(true);
	}

	public void OnUnequip(Item.Data item)
    {
		OnUnequip(item.meta.equip.part);
    }

	public void OnUnequip(Message.EquipItemPartType part)
	{
		suit_images[(int)part].gameObject.SetActive(false);
	}
}
