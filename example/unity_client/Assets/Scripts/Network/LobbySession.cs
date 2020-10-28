using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Network
{
	public class LobbySession : Gamnet.Session
	{
		public LobbySession()
		{
		}

		private void OnApplicationPause(bool pause)
		{
			if (true == pause)
			{
				Pause();
			}
			else
			{
				Resume();
			}
		}

		private void OnApplicationQuit()
		{
			base.Close();
		}
	}
}
