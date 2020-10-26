using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace UI
{
    public class Main : MonoBehaviour
    {
        public Transform canvas;

        private Dictionary<string, GameObject> ui_objects = new Dictionary<string, GameObject>();
        // Start is called before the first frame update
        void Start()
        {
            {
                Transform prefab = GameManager.Instance.resource.Load<Transform>("Prefabs/Canvas");
                canvas = GameObject.Instantiate(prefab);
                canvas.SetParent(transform, false);
            }

            {
                MailBox prefab = GameManager.Instance.resource.Load<MailBox>("Prefabs/MailBox");
                MailBox obj = GameObject.Instantiate<MailBox>(prefab);
                obj.transform.SetParent(canvas, false);
                ui_objects.Add("MailBox", obj.gameObject);
                SetActive("MailBox", false);
            }
        }

        // Update is called once per frame
        void Update()
        {

        }

        void SetActive(string key, bool flag)
        {
            if (true == ui_objects.ContainsKey(key))
            {
                GameObject obj = ui_objects[key];
                obj.SetActive(flag);
            }
        }
    }
}