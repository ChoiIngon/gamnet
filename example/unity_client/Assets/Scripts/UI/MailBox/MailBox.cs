using System;
using UnityEngine;

namespace UI
{
    public class MailBox : MonoBehaviour
    {
		public class Event
		{
			public const string AddMail = "UI.MailBox.Event.AddMail";
			public const string RemoveMail = "UI.MailBox.Event.RemoveMail";
		}
		public Mail mail_prefab;
		public Transform contents;
		public const int MAX_MAIL_COUNT = 30;
		private void Awake()
		{
			Util.EventSystem.Subscribe<Message.MailData>(Event.AddMail, OnAddMail);
			Util.EventSystem.Subscribe<UInt64>(Event.RemoveMail, OnRemoveMail);

			for (int i = 0; i < MAX_MAIL_COUNT; i++)
			{
				Mail mail = GameObject.Instantiate<Mail>(mail_prefab);
				mail.transform.SetParent(contents, false);
				mail.gameObject.SetActive(false);
			}
		}

		private void OnDestroy()
		{
			Util.EventSystem.Unsubscribe<Message.MailData>(Event.AddMail);
			Util.EventSystem.Unsubscribe<UInt64>(Event.RemoveMail);
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
		}

		private void OnDisable()
		{
			
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
			else
			{
				while (0 < contents.childCount)
				{
					Transform child = contents.GetChild(0);
					child.SetParent(null);
					GameObject.Destroy(child.gameObject);
				}
			}
		}
	}
}