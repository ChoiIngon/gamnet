namespace Handler
{
	namespace User
	{
		class Handler_Counter
		{
			public static void OnRecv(Message.User.MsgSvrCli_Counter_Ntf ntf)
			{
				foreach (Message.CounterData counter in ntf.counter_datas)
				{
					GameManager.Instance.counter.AddCounter(counter);
				}
			}
		}
	}
}
