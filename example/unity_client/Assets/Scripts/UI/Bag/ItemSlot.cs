using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class ItemSlot : MonoBehaviour
    {
        public Item.Data item;
        public Image grade;
        public Image icon;
        public Image border;
        public Text item_name;
        public Button use_button;
        public Button sell_button;

        private void Awake()
        {
            if (null == use_button)
            {
                return;
            }
            use_button.onClick.AddListener(() =>
            {
                switch (item.meta.type)
                {
                    case Item.Meta.ItemType.Package:
                        GameManager.Instance.bag.Send_OpenPackage_Req(item.seq);
                        break;
                    case Item.Meta.ItemType.Equip:
                        if (Message.EquipItemPartType.Invalid == item.equip.part)
                        {
                            GameManager.Instance.bag.Send_EquipItem_Req(item.seq);
                        }
                        else
                        {
                            GameManager.Instance.bag.Send_UnequipItem_Req(item.meta.equip.part);
                        }
                        break;
                }
            });
        }
		
		public void SetItemData(Item.Data item)
        {
            this.item = item;
            item_name.text = item.meta.name + " x " + item.count.ToString();
            icon.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(item.meta.icon_path);
        }


    }
}