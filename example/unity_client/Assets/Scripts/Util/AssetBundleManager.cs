using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Networking;
#if UNITY_EDITOR
using UnityEditor;
#endif

public class AssetBundleManager : Util.MonoSingleton<AssetBundleManager>
{
	private Dictionary<string, AssetBundle> asset_bundles = new Dictionary<string, AssetBundle>();
	private Dictionary<string, AssetBundle> asset_name_to_bundle = new Dictionary<string, AssetBundle>();
	private AssetBundleManifest manifest;
	public string url;
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
		string manifestName = "Windows";
#if UNITY_ANDROID
		manifestName = "Android";
#endif
#if UNITY_IOS
		manifestName = "iOS";
#endif
		string assetBundleRepository = GetAssetBundleRepository(manifestName);
		UnityWebRequest request = UnityWebRequestAssetBundle.GetAssetBundle(assetBundleRepository, 0);
		if (null == request)
		{
			throw new System.Exception("Failed to request(url:" + assetBundleRepository + ")");
		}

		yield return request.SendWebRequest();

		AssetBundle assetBundle = DownloadHandlerAssetBundle.GetContent(request);
		if (null == assetBundle)
		{
			throw new System.Exception("Failed to load asset bundle(url:" + assetBundleRepository + ",  bundle_name:" + manifestName + ")");
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
		assetBundleName = assetBundleName.ToLower();
		if (true == asset_bundles.ContainsKey(assetBundleName))
		{
			yield break;
		}

		string assetBundleRepository = GetAssetBundleRepository(assetBundleName);

		Hash128 hash = manifest.GetAssetBundleHash(assetBundleName);
		UnityWebRequest request = UnityWebRequestAssetBundle.GetAssetBundle(assetBundleRepository, hash, 0);
		yield return request.SendWebRequest();
		AssetBundle assetBundle = DownloadHandlerAssetBundle.GetContent(request);
		if (null == request)
		{
			throw new System.Exception("Failed to request(url:" + url + ")");
		}
		
		string [] assetNames = assetBundle.GetAllAssetNames();
		foreach (string fullAssetName in assetNames)
		{
			string assetName = Path.ChangeExtension(fullAssetName.ToLower(), null);
			Debug.Log("asset name:" + assetName);
			asset_name_to_bundle.Add(assetName, assetBundle);
		}
		asset_bundles.Add(assetBundleName, assetBundle);
	}
	private string GetAssetBundleRepository(string assetBundleName)
	{
		string assetBundleRepository = url;
#if UNITY_EDITOR
		if (true == EditorPrefs.GetBool(Menu_Local, false))
		{
			assetBundleRepository = "file:///" + Application.dataPath;
		}
#endif
		string platform = "Windows";
#if UNITY_ANDROID
		platform = "Android";
#endif
#if UNITY_IOS
		platform = "iOS";
#endif
		assetBundleRepository += "/AssetBundles/" + platform + "/" + assetBundleName;
		Debug.Log("asset bundle url:" + assetBundleRepository);
		return assetBundleRepository;
	}

	public T LoadAsset<T>(string assetName) where T : Object
	{
		assetName = assetName.ToLower();
		if (false == asset_name_to_bundle.ContainsKey(assetName))
		{
			return null;
		}

		AssetBundle assetBundle = asset_name_to_bundle[assetName];

		assetName = Path.GetFileName(assetName);
		return assetBundle.LoadAsset<T>(assetName);
	}
#if UNITY_EDITOR
	[MenuItem("AssetBundle/Build/Windows", priority = 1)]
	static void BuildUnity()
	{
		Build(BuildTarget.StandaloneWindows, "Windows");
	}
	[MenuItem("AssetBundle/Build/Android", priority = 1)]
	static void BuildAndroid()
	{
		Build(BuildTarget.Android, "Android");
	}
	[MenuItem("AssetBundle/Build/iOS", priority = 1)]
	static void BuildiOS()
	{
		Build(BuildTarget.iOS, "iOS");
	}

	static void Build(BuildTarget buildTarget, string outputPath)
	{
		string assetBundleDirectory = "Assets/AssetBundles/" + outputPath;
		if (false == Directory.Exists(assetBundleDirectory))
		{
			Directory.CreateDirectory(assetBundleDirectory);
		}

		BuildPipeline.BuildAssetBundles(assetBundleDirectory, BuildAssetBundleOptions.None, buildTarget);
		AssetDatabase.Refresh();
	}

	const string Menu_Local = "AssetBundle/Local";
	[MenuItem(Menu_Local)]
	static void Local()
	{
		bool enable = EditorPrefs.GetBool(Menu_Local, false);
		Menu.SetChecked(Menu_Local, !enable);
		EditorPrefs.SetBool(Menu_Local, !enable);
	}
#endif
}
