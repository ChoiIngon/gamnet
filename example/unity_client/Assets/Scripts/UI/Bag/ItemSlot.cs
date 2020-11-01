using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class ItemSlot : MonoBehaviour
    {
        private Message.ItemData item;
        public Image grade;
        public Image icon;
        public Image border;
        public Text item_name;
        public Button use_button;
        public Button sell_button;
        public void SetItemData(Message.ItemData item)
        {
            Item.Meta meta = Item.Manager.Instance.FindMeta(item.item_index);

            this.item = item;
            item_name.text = meta.name + " x " + item.item_count.ToString();
            icon.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(meta.icon_path);
        }
    }
}