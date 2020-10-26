using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Component
{
	class MailBox : IEnumerable
	{
		public MailBox()
		{
			GameManager.Instance.session.RegisterHandler<Message.Lobby.MsgSvrCli_Mail_Ntf>(AddMail);
		}

		~MailBox()
		{
			GameManager.Instance.session.UnregisterHandler<Message.Lobby.MsgSvrCli_Mail_Ntf>(AddMail);
		}

		public void AddMail(Message.Lobby.MsgSvrCli_Mail_Ntf ntf)
		{
			foreach (Message.MailData mail in ntf.mail_datas)
			{
				AddMail(mail);
			}
		}
		public Message.MailData AddMail(Message.MailData data)
		{
			Debug.Log("recv mail:" + data.mail_message);
			mail_datas.Add(data.mail_seq, data);
			Util.EventSystem.Publish<Message.MailData>(UI.MailBox.Event.AddMail, data);
			return data;
		}

		public void OpenMail(UInt64 mailSEQ)
		{
			GameManager.Instance.session.RegisterHandler<Message.Lobby.MsgSvrCli_OpenMail_Ans>(RemoveMail);
			Message.Lobby.MsgCliSvr_OpenMail_Req req = new Message.Lobby.MsgCliSvr_OpenMail_Req();
			req.mail_seq = mailSEQ;
			GameManager.Instance.session.SendMsg(req, true);
		}

		public void RemoveMail(Message.Lobby.MsgSvrCli_OpenMail_Ans ans)
		{
			GameManager.Instance.session.UnregisterHandler<Message.Lobby.MsgSvrCli_OpenMail_Ans>(RemoveMail);

			if (Message.ErrorCode.Success != ans.error_code)
			{
				return;
			}

			if (0 == ans.mail_seq)
			{
				Clear();
			}
			else
			{
				RemoveMail(ans.mail_seq);
			}
			Util.EventSystem.Publish<UInt64>(UI.MailBox.Event.RemoveMail, ans.mail_seq);
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
