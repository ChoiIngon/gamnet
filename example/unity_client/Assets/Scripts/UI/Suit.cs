using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Suit : MonoBehaviour
{
    public Image[] items = new Image[(int)Message.EquipItemPartType.Max];

	private void OnEnable()
	{
		Util.EventSystem.Subscribe<Item.Data>(Component.Bag.Event.EquipItem, OnEquip);
		Util.EventSystem.Subscribe<Message.EquipItemPartType>(Component.Bag.Event.UnequipItem, OnUnequip);
	}

	private void OnDisable()
	{
		Util.EventSystem.Unsubscribe<Item.Data>(Component.Bag.Event.EquipItem, OnEquip);
		Util.EventSystem.Unsubscribe<Message.EquipItemPartType>(Component.Bag.Event.UnequipItem, OnUnequip);
	}

	public void OnEquip(Item.Data item)
    {
        items[(int)item.meta.equip.part].sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(item.meta.equip.sprite_path);
    }

    public void OnUnequip(Message.EquipItemPartType part)
    {
        items[(int)part].sprite = null;
    }
}
