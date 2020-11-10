using System;
using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class Bag : MonoBehaviour
    {
        public ItemSlot itemslot_prefab;
        public Transform itemslot_contents;
        public Button close_button;

        private void Awake()
        {
            close_button.onClick.AddListener(() =>
            {
                gameObject.SetActive(false);
            });
        }
        private void OnEnable()
        {
			int childIndex = 0;
            foreach (Item.Data data in GameManager.Instance.bag)
            {
				ItemSlot itemSlot = null;
				
				if (childIndex < itemslot_contents.childCount)
				{
					Transform child = itemslot_contents.GetChild(childIndex);
					itemSlot = child.GetComponent<ItemSlot>();
				}
				else
				{
					itemSlot = GameObject.Instantiate<ItemSlot>(itemslot_prefab);
					itemSlot.transform.SetParent(itemslot_contents, false);
				}
                itemSlot.SetItemData(data);
				childIndex++;
            }

			while (childIndex < itemslot_contents.childCount)
			{
				Transform child = itemslot_contents.GetChild(childIndex);
				child.SetParent(null);
				GameObject.Destroy(child.gameObject);
			}

			Util.EventSystem.Subscribe<Item.Data>(Component.Bag.Event.AddItem, OnAddItem);
			Util.EventSystem.Subscribe<Item.Data>(Component.Bag.Event.UpdateItem, OnUpdateItem);
			Util.EventSystem.Subscribe<UInt64>(Component.Bag.Event.RemoveItem, OnRemoveItem);
		}
		private void OnDisable()
		{
			Util.EventSystem.Unsubscribe<Item.Data>(Component.Bag.Event.AddItem, OnAddItem);
			Util.EventSystem.Unsubscribe<Item.Data>(Component.Bag.Event.UpdateItem, OnUpdateItem);
			Util.EventSystem.Unsubscribe<UInt64>(Component.Bag.Event.RemoveItem, OnRemoveItem);
		}
		private void OnAddItem(Item.Data data)
		{
			ItemSlot itemSlot = GameObject.Instantiate<ItemSlot>(itemslot_prefab);
			itemSlot.transform.SetParent(itemslot_contents, false);
			itemSlot.SetItemData(data);
		}
		private void OnUpdateItem(Item.Data data)
		{

		}
		private void OnRemoveItem(UInt64 itemSEQ)
		{
			foreach (Transform child in itemslot_contents.transform)
			{
				ItemSlot itemSlot = child.gameObject.GetComponent<ItemSlot>();
				if (null != itemSlot)
				{
					if (itemSEQ == itemSlot.item.seq)
					{
						child.SetParent(null);
						GameObject.Destroy(child.gameObject);
						return;
					}
				}
			}
		}

	}

}