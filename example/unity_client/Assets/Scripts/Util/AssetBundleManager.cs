using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Networking;

public class AssetBundleManager : Util.MonoSingleton<AssetBundleManager>
{
	private Dictionary<string, AssetBundle> asset_bundles = new Dictionary<string, AssetBundle>();
	private Dictionary<string, AssetBundle> asset_name_to_bundle = new Dictionary<string, AssetBundle>();
	private AssetBundleManifest manifest;
	private void Awake()
	{
		DontDestroyOnLoad(transform.gameObject);
	}

	public IEnumerator LoadAssetBundle(string assetBundleName)
	{
		if (null == manifest)
		{
			yield return LoadManifest();
		}
		Debug.Log("start to load asset:" + assetBundleName);
		string[] dependencies = manifest.GetAllDependencies(assetBundleName);
		foreach (string dependency in dependencies)
		{
			Debug.Log(dependency);
			yield return LoadAssetBundle(dependency);
		}
		yield return _LoadAssetBundle(assetBundleName);
	}

	private IEnumerator LoadManifest()
	{
		Debug.Log("start to load AssetBundle manifest");
		string manifestName = "Unity";
#if UNITY_ANDROID
		manifestName = "Android";
#endif
#if UNITY_IOS
		manifestName = "iOS";
#endif
		//string url = "file:///" + Application.dataPath + "/AssetBundles/Unity/" + assetBundleName.ToLower();
		string url = "http://assetbundles-ygmwl.run.goorm.io/AssetBundles/Unity/" + manifestName;
#if UNITY_ANDROID
		url = "http://assetbundles-ygmwl.run.goorm.io/AssetBundles/Android/" +  + manifestName;
#endif
#if UNITY_IOS
		url = "http://assetbundles-ygmwl.run.goorm.io/AssetBundles/iOS/" +  + manifestName;
#endif
		UnityWebRequest request = UnityWebRequestAssetBundle.GetAssetBundle(url, 0);
		yield return request.SendWebRequest();
		AssetBundle assetBundle = DownloadHandlerAssetBundle.GetContent(request);
		if (null == request)
		{
			throw new System.Exception("Failed to request(url:" + url + ")");
		}

		manifest = assetBundle.LoadAsset<AssetBundleManifest>("AssetBundleManifest");
		if (null == manifest)
		{
			throw new System.Exception("Failed to load asset bundle manifest");
		}

		asset_bundles.Add(manifestName, assetBundle);
	}

	private IEnumerator _LoadAssetBundle(string assetBundleName)
	{
		if (true == asset_bundles.ContainsKey(assetBundleName.ToLower()))
		{
			yield break;
		}

		//string url = "file:///" + Application.dataPath + "/AssetBundles/Unity/" + assetBundleName.ToLower();
		string url = "http://assetbundles-ygmwl.run.goorm.io/AssetBundles/Unity/" + assetBundleName.ToLower();
#if UNITY_ANDROID
		url = "http://assetbundles-ygmwl.run.goorm.io/AssetBundles/Android/" + assetBundleName.ToLower();
#endif
#if UNITY_IOS
		url = "http://assetbundles-ygmwl.run.goorm.io/AssetBundles/iOS/" + assetBundleName.ToLower();
#endif
		UnityWebRequest request = UnityWebRequestAssetBundle.GetAssetBundle(url, 0);
		yield return request.SendWebRequest();
		AssetBundle assetBundle = DownloadHandlerAssetBundle.GetContent(request);
		if (null == request)
		{
			throw new System.Exception("Failed to request(url:" + url + ")");
		}
		
		string [] assetNames = assetBundle.GetAllAssetNames();
		foreach (string fullAssetName in assetNames)
		{
			string assetName = Path.GetFileNameWithoutExtension(fullAssetName).ToLower();
			Debug.Log("asset name:" + assetName);
			asset_name_to_bundle.Add(assetName, assetBundle);
		}
		asset_bundles.Add(assetBundleName.ToLower(), assetBundle);
	}

	public T LoadAsset<T>(string assetName) where T : Object
	{
		if (false == asset_name_to_bundle.ContainsKey(assetName.ToLower()))
		{
			return null;
		}

		AssetBundle assetBundle = asset_name_to_bundle[assetName.ToLower()];
		return assetBundle.LoadAsset<T>(assetName.ToLower());
	}


	// Update is called once per frame
	void Update()
    {
        
    }	
}
