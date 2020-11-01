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
            foreach (Message.ItemData data in GameManager.Instance.bag)
            {
                ItemSlot itemSlot = GameObject.Instantiate<ItemSlot>(itemslot_prefab);
                itemSlot.transform.SetParent(itemslot_contents, false);
                itemSlot.SetItemData(data);
            }
        }

        // Update is called once per frame
        void Update()
        {

        }


    }

}