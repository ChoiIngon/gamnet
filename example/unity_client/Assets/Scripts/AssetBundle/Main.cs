using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Networking;

public class Main : MonoBehaviour
{
	// Start is called before the first frame update
	IEnumerator Start()
    {
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
