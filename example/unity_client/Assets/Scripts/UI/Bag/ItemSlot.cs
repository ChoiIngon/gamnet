using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class ItemSlot : MonoBehaviour
    {
        public abstract class Strategy
        {
            protected ItemSlot item_slot;
            public Strategy(ItemSlot itemSlot)
            {
                item_slot = itemSlot;
            }
            public abstract void OnSetItem();
            public abstract void OnUseButton();
            public abstract void OnSellButton();
        };
        public class EquipItem : Strategy
        {
            private const string EQUIP = "장착";
            private const string UNEQUIP = "해제";
            public EquipItem(ItemSlot itemSlot) : base(itemSlot)
            {
                Util.EventSystem.Subscribe<Item.Data>(Component.Suit.Event.EquipItem, OnEquip);
                Util.EventSystem.Subscribe<Item.Data>(Component.Suit.Event.UnequipItem, OnUnequip);
            }

            ~EquipItem()
            {
                Util.EventSystem.Unsubscribe<Item.Data>(Component.Suit.Event.EquipItem, OnEquip);
                Util.EventSystem.Unsubscribe<Item.Data>(Component.Suit.Event.UnequipItem, OnUnequip);
            }
            public override void OnSetItem()
            {
                if (Message.EquipItemPartType.Invalid == item_slot.item.equip.part)
                {
                    item_slot.use_button_text.text = EQUIP;
                }
                else
                {
                    item_slot.use_button_text.text = UNEQUIP;
                }
            }
            public override void OnUseButton()
            {
                if (Message.EquipItemPartType.Invalid == item_slot.item.equip.part)
                {
                    GameManager.Instance.bag.Send_EquipItem_Req(item_slot.item.seq);
                }
                else
                {
                    GameManager.Instance.bag.Send_UnequipItem_Req(item_slot.item.meta.equip.part);
                }
            }

            public override void OnSellButton()
            {
                GameManager.Instance.bag.Send_SellItem_Req(item_slot.item);
            }
            private void OnEquip(Item.Data item)
            {
                if (item != item_slot.item)
                {
                    return;
                }
                item_slot.use_button_text.text = UNEQUIP;
            }

            private void OnUnequip(Item.Data item)
            {
                if (item != item_slot.item)
                {
                    return;
                }
                item_slot.use_button_text.text = EQUIP;
            }
        };
        public class PackageItem : Strategy
        {
            public PackageItem(ItemSlot itemSlot) : base(itemSlot)
            {
            }
            public override void OnSetItem()
            {
                item_slot.use_button_text.text = "사용";
            }
            public override void OnUseButton()
            {
                GameManager.Instance.bag.Send_OpenPackage_Req(item_slot.item.seq);
            }
            public override void OnSellButton()
            {
                GameManager.Instance.bag.Send_SellItem_Req(item_slot.item);
            }
        };

        public Item.Data item;
        public Image grade;
        public Image icon;
        public Image border;
        public Text item_name;
        public Button use_button;
        public Button sell_button;
        public Text use_button_text;
        private Strategy strategy;
        private void Awake()
        {
            use_button_text = use_button.GetComponentInChildren<Text>();
            use_button.onClick.AddListener(() =>
            {
                strategy.OnUseButton();
            });
            sell_button.onClick.AddListener(() =>
            {
                strategy.OnSellButton();
            });
        }
		
		public void SetItemData(Item.Data item)
        {
            this.item = item;
            item_name.text = item.meta.name + " x " + item.count.ToString();
            icon.sprite = item.meta.icon_sprite;

            switch (item.meta.type)
            {
                case Item.Meta.ItemType.Package:
                    strategy = new PackageItem(this);
                    break;
                case Item.Meta.ItemType.Equip:
                    strategy = new EquipItem(this);
                    break;
            }

            strategy.OnSetItem();
        }

       
    }
}