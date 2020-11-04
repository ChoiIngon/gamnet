using System;
using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class MailBox : MonoBehaviour
    {
		public Mail mail_prefab;
		private Transform contents = null;
		private const int MAX_MAIL_COUNT = 30;
		public Button close_button;
		public Button recv_all_button;
		private void Awake()
		{
			contents = transform.Find("ScrollRect/Viewport/Content");

			close_button.onClick.AddListener(() =>
			{
				this.gameObject.SetActive(false);
			});
			recv_all_button.onClick.AddListener(() =>
			{
				GameManager.Instance.mailbox.OpenMail(0);
			});
			for (int i = 0; i < MAX_MAIL_COUNT; i++)
			{
				Mail mail = GameObject.Instantiate<Mail>(mail_prefab);
				mail.transform.SetParent(contents, false);
				mail.gameObject.SetActive(false);
			}
		}
		private void OnEnable()
		{
			int childIndex = 0;
			foreach (Message.MailData data in GameManager.Instance.mailbox)
			{
				if (MAX_MAIL_COUNT <= childIndex)
				{
					break;
				}
				
				Mail mail = contents.GetChild(childIndex).GetComponent<Mail>();
				mail.SetMailData(data);
				mail.gameObject.SetActive(true);
				childIndex++;
			}

			for (; childIndex < MAX_MAIL_COUNT; childIndex++)
			{
				Mail mail = contents.GetChild(childIndex).GetComponent<Mail>();
				mail.gameObject.SetActive(false);
			}
			Util.EventSystem.Subscribe<Message.MailData>(Component.MailBox.Event.AddMail, OnAddMail);
			Util.EventSystem.Subscribe<UInt64>(Component.MailBox.Event.RemoveMail, OnRemoveMail);
		}
		private void OnDisable()
		{
			Util.EventSystem.Unsubscribe<Message.MailData>(Component.MailBox.Event.AddMail);
			Util.EventSystem.Unsubscribe<UInt64>(Component.MailBox.Event.RemoveMail);
		}
		private void OnAddMail(Message.MailData data)
		{
			Mail mail = GameObject.Instantiate<Mail>(mail_prefab);
			mail.SetMailData(data);
			mail.transform.SetParent(contents);
		}
		private void OnRemoveMail(UInt64 mailSEQ)
		{
			if (0 == mailSEQ)
			{
				while (0 < contents.childCount)
				{
					Transform child = contents.GetChild(0);
					child.SetParent(null);
					GameObject.Destroy(child.gameObject);
				}
			}
			else
			{
				foreach (Transform child in contents.transform)
				{
					Mail mail = child.gameObject.GetComponent<Mail>();
					if (null != mail)
					{
						if (mailSEQ == mail.mail_seq)
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
}