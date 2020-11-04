using UnityEditor;
using UnityEngine;
using System.Collections.Generic;

namespace Editor.Menu
{
	public class BuildIDL
	{
		[MenuItem("Custom/IDL/Build")]
		static void Build()
		{
			string idlc_path = System.IO.Path.Combine(Application.dataPath, "../../../x64/Debug");
			string message_path = System.IO.Path.Combine(Application.dataPath, "../../idl");

			if (Application.platform == RuntimePlatform.WindowsEditor)
			{
				List<string> files = new List<string>() {
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
				AssetDatabase.Refresh();
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
	}
}