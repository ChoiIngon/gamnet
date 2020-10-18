
namespace Handler.Lobby	{
	class Handler_Mail
	{
		public static void OnRecv(Message.Lobby.MsgSvrCli_Mail_Ntf ntf)
		{
			foreach (Message.MailData mail in ntf.mail_datas)
			{
				GameManager.Instance.mail.AddMail(mail);
			}
		}
	}
}
