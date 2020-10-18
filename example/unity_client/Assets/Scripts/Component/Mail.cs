using System;
using System.Collections;
using System.Collections.Generic;

namespace Component
{
	class Mail : IEnumerable
	{
		public Message.MailData AddMail(Message.MailData data)
		{
			mail_datas.Add(data.mail_seq, data);
			return data;
		}
		public Message.MailData GetMail(UInt64 mailSEQ)
		{
			Message.MailData data = null;
			if (false == mail_datas.TryGetValue(mailSEQ, out data))
			{
				return null;
			}
			return data;
		}

		public void RemoveMail(UInt64 mailSEQ)
		{
			mail_datas.Remove(mailSEQ);
		}
		public void Clear()
		{
			mail_datas = new Dictionary<ulong, Message.MailData>();
		}
		public IEnumerator GetEnumerator()
		{
			foreach (var itr in mail_datas)
			{
				Message.MailData mail = itr.Value;
				yield return mail;
			}
		}
		private Dictionary<UInt64, Message.MailData> mail_datas = new Dictionary<UInt64, Message.MailData>();
	}
}
