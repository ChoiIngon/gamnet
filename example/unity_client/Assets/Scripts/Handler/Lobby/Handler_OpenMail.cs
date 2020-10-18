using System;

namespace Handler.Lobby
{
	class Handler_OpenMail
	{
		public static void SendMsg(UInt64 mailSEQ)
		{
			GameManager.Instance.session.RegisterHandler<Message.Lobby.MsgSvrCli_OpenMail_Ans>(Handler_OpenMail.OnRecv);

			Message.Lobby.MsgCliSvr_OpenMail_Req req = new Message.Lobby.MsgCliSvr_OpenMail_Req();
			req.mail_seq = mailSEQ;
			GameManager.Instance.session.SendMsg(req, true);
		}

		public static void OnRecv(Message.Lobby.MsgSvrCli_OpenMail_Ans ans)
		{
			GameManager.Instance.session.UnregisterHandler<Message.Lobby.MsgSvrCli_OpenMail_Ans>(Handler_OpenMail.OnRecv);

			if (Message.ErrorCode.Success != ans.error_code)
			{
				return;
			}

			if (0 == ans.mail_seq)
			{
				GameManager.Instance.mail.Clear();
			}
			else
			{
				GameManager.Instance.mail.RemoveMail(ans.mail_seq);
			}
		}
	}
}
