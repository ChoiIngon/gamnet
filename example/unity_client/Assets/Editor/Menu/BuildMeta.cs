using UnityEditor;
using UnityEngine;
using System.Collections.Generic;
using System.IO;

namespace Ediotr.Menu
{
	public class BuildMeta
	{
		[MenuItem("Custom/Meta/Build")]
		static void Build()
		{
			string srcPath = System.IO.Path.Combine(Application.dataPath, "../../MetaData");
			string destPath = Application.dataPath + "/MetaData";
			if (false == Directory.Exists(destPath))
			{
				Directory.CreateDirectory(destPath);
				AssetDatabase.Refresh();
				AssetImporter.GetAtPath("Assets/MetaData").SetAssetBundleNameAndVariant("MetaData", "");
			}

			List<string> files = new List<string>()
			{
				"Dungeon",
				"EquipItem",
				"Item",
				"Monster"
			};

			foreach (string file in files)
			{
				string srcFile = srcPath + "/" + file + ".csv";
				string destFile = destPath + "/" + file + ".csv";
				File.Copy(srcFile, destFile , true);
			}
			AssetDatabase.Refresh();

			Debug.Log("Complete build MetaData");
		}
	}
}