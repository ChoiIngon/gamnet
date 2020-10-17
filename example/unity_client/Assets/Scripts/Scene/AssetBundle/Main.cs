using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Networking;

namespace Scene 
{ 
	namespace AssetBundle	
	{
		public class Main : MonoBehaviour
		{
			IEnumerator Start()
		{
			yield return Item.Manager.Instance.Init();

			{
				Item.Meta meta = Item.Manager.Instance.FindMeta("ITEM_001");
				if (null != meta)
				{
					Debug.Log(meta);
				}
				Item item = meta.CreateInstance();
				item.transform.SetParent(this.transform);
			}

			yield return AssetBundleManager.Instance.LoadAssetBundle("AssetBundle1");
			{
				var prefab = AssetBundleManager.Instance.LoadAsset<GameObject>("AssetBundle1_1");
				Instantiate(prefab);
			}
			{
				var prefab = AssetBundleManager.Instance.LoadAsset<GameObject>("AssetBundle1_2");
				Instantiate(prefab);
			}
			yield return AssetBundleManager.Instance.LoadAssetBundle("AssetBundle2");
			{
				var prefab = AssetBundleManager.Instance.LoadAsset<GameObject>("AssetBundle2_1");
				Instantiate(prefab);
			}
			{
				var prefab = AssetBundleManager.Instance.LoadAsset<GameObject>("AssetBundleChild");
				Instantiate(prefab);
			}
		}
			void Update()
			{
			}
			public static string GetName()
			{
				return "SceneAssetBundle";
			}
		}
	}
}