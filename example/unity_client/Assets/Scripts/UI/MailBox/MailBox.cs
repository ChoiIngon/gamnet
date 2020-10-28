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

		private void Start()
		{
			Util.EventSystem.Subscribe<Message.MailData>(Event.AddMail, OnAddMail);
			Util.EventSystem.Subscribe<UInt64>(Event.RemoveMail, OnRemoveMail);
		}

		private void OnDestroy()
		{
			Util.EventSystem.Unsubscribe<Message.MailData>(Event.AddMail);
			Util.EventSystem.Unsubscribe<UInt64>(Event.RemoveMail);
		}

		private void OnEnable()
		{
			int childIndex = 0;
			/*
			foreach (Message.MailData data in GameManager.Instance.mailbox)
			{
				Mail mail = null;
				Transform child = contents.GetChild(childIndex);
				if (null == child)
				{
					mail = GameObject.Instantiate<Mail>(mail_prefab);
					mail.transform.SetParent(contents, false);
				}
				else
				{
					mail = child.gameObject.GetComponent<Mail>();
				}
				mail.SetMailData(data);
			}
			*/
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