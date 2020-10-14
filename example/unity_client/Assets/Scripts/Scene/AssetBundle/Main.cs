using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Networking;

public class Main : MonoBehaviour
{
	// Start is called before the first frame update
	public class ItemMeta : MetaData
	{
		public int index;
	}
	IEnumerator Start()
    {
		yield return AssetBundleManager.Instance.LoadAssetBundle("MetaData");

		List<string> list = new List<string>();
		if (typeof(IEnumerable) == list.GetType())
		{
			Debug.Log("test");
		}
		{
			TextAsset textAsset = AssetBundleManager.Instance.LoadAsset<TextAsset>("Item");
			CSVReader reader = new CSVReader();
			reader.ReadStream(textAsset.text);
			MetaData.Reader<ItemMeta> metaReader = new MetaData.Reader<ItemMeta>();
			metaReader.Read(reader);

			
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

	// Update is called once per frame
	void Update()
    {
        
    }
}
