using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace UI
{
    public class UIMain : MonoBehaviour
    {
        public const string UIKey_MailBox = "UIKey_MailBox";
        public const string UIKey_Login = "UIKey_Login";
        public const string UIKey_Lobby = "UIKey_Lobby";

        public Transform canvas;
        private Dictionary<string, GameObject> ui_objects = new Dictionary<string, GameObject>();
        // Start is called before the first frame update
        void Awake()
        {
            {
                Transform child = canvas.Find("Login");
                ui_objects.Add(UIKey_Login, child.gameObject);
                SetActive(UIKey_Login, false);
            }

            {
                Transform child = canvas.Find("Lobby");
                ui_objects.Add(UIKey_Lobby, child.gameObject);
                SetActive(UIKey_Lobby, false);
            }

            {
                MailBox prefab = ResourceManager.Instance.Load<MailBox>("Prefabs/MailBox");
                MailBox obj = GameObject.Instantiate<MailBox>(prefab);
                obj.transform.SetParent(canvas, false);
                ui_objects.Add(UIKey_MailBox, obj.gameObject);
                SetActive(UIKey_MailBox, false);
            }
        }

        public void SetActive(string key, bool flag)
        {
            if (true == ui_objects.ContainsKey(key))
            {
                GameObject obj = ui_objects[key];
                obj.SetActive(flag);
            }
        }
    }
}