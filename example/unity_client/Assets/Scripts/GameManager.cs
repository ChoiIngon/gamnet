using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;

class GameManager : Util.MonoSingleton<GameManager>
{
	public class Scenes
	{
		public LobbyMain lobby_main = null;
		public DungeonMain dungeon_main = null;
	}

	public Gamnet.Session session = new Gamnet.Session();
	public bool run_in_background = false;
	public Scenes scenes = new Scenes();
	
	private void Awake()
	{
		DontDestroyOnLoad(transform.gameObject);
		if (true == Application.isEditor)
		{
			Application.runInBackground = run_in_background;
		}

		session.onError += (Gamnet.Exception e) =>
		{
			if (Gamnet.ErrorCode.ReconnectFailError == e.ErrorCode)
			{
				session.Close();
				SceneManager.LoadScene("SceneLobby");
			}
		};
		session.RegisterHandler<Message.User.MsgSvrCli_Counter_Ntf>(Handler.User.Handler_Counter.OnRecv);
		session.RegisterHandler<Message.Item.MsgSvrCli_Item_Ntf>(Handler.Item.Handler_Item.OnRecv);
	}

	private void Start()
	{
	}

	private void OnApplicationPause(bool pause)
	{
		if (true == pause)
		{
			session.Pause();
		}
		else
		{
			session.Resume();
		}
	}

	private void OnApplicationQuit()
	{
		session.Close();
	}
	[MenuItem("IDL/Build")]
	private static void BuildIDL()
	{
		string idlc_path = System.IO.Path.Combine(Application.dataPath, "../../../x64/Debug");
		string message_path = System.IO.Path.Combine(Application.dataPath, "../../idl");

		if (Application.platform == RuntimePlatform.WindowsEditor)
		{
			List<string> files = new List<string>()
			{
				"MessageCommon",
				"MessageDungeon",
				"MessageItem",
				"MessageLobby",
				"MessageUser"
			};

			foreach (string file in files)
			{
				System.Diagnostics.Process.Start(idlc_path + "/idlc", "/K -lcs " + message_path + "/" + file + ".idl");
				System.IO.File.Copy(message_path + "/" + file + ".cs", Application.dataPath + "/Scripts/IDL/" + file + ".cs", true);
			}
		}
		/*
		else if(Application.platform == RuntimePlatform.OSXEditor)
		{
			Process.Start(Application.dataPath + "/Script/Network/IDL/idlc", "/K -lcs " + Application.dataPath + "/Script/Network/IDL/idl");
			Process.Start(Application.dataPath + "/Script/Network/IDL/idlc", "/K -lcs " + Application.dataPath + "/Script/Network/IDL/XXError.idl");
			Process.Start(Application.dataPath + "/Script/Network/IDL/idlc", "/K -lcs " + Application.dataPath + "/Script/Network/IDL/XXMessage.idl");
			Process.Start(Application.dataPath + "/Script/Network/IDL/idlc", "/K -lcs " + Application.dataPath + "/Script/Network/IDL/P2PMessage.idl");
		}
        */
	}
	void Update()
	{
		session.Update();
	}
}

