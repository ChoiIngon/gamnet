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
		string manifestName = "Unity";
#if UNITY_ANDROID
		manifestName = "Android";
#endif
#if UNITY_IOS
		manifestName = "iOS";
#endif
		string assetBundleRepository = GetAssetBundleRepository(manifestName);
		UnityWebRequest request = UnityWebRequestAssetBundle.GetAssetBundle(assetBundleRepository, 0);
		yield return request.SendWebRequest();
		AssetBundle assetBundle = DownloadHandlerAssetBundle.GetContent(request);
		if (null == request)
		{
			throw new System.Exception("Failed to request(url:" + assetBundleRepository + ")");
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

		string assetBundleRepository = GetAssetBundleRepository(assetBundleName.ToLower());

		UnityWebRequest request = UnityWebRequestAssetBundle.GetAssetBundle(assetBundleRepository, 0);
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
	private string GetAssetBundleRepository(string assetBundleName)
	{
		string assetBundleRepository = url;
#if UNITY_ANDROID
		assetBundleRepository += "/AssetBundles/Android/" + assetBundleName;
#endif
#if UNITY_IOS
		assetBundleRepository += "/AssetBundles/iOS/" + assetBundleName;
#endif
#if UNITY_EDITOR
		if (true == EditorPrefs.GetBool(Menu_LocalServer, false))
		{
			string path = "/AssetBundles/Unity/";
			if (true == EditorPrefs.GetBool(Menu_PlatformAndroid))
			{
				path = "/AssetBundles/Android/";
			}
			else if (true == EditorPrefs.GetBool(Menu_PlatformiOS))
			{
				path = "/AssetBundles/iOS/";
			}
			assetBundleRepository = "file:///" + Application.dataPath + path + assetBundleName;
		}
#endif
		return assetBundleRepository;
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
#if UNITY_EDITOR
	const string Menu_LocalServer = "AssetBundle/LocalServer";
	const string Menu_PlatformEditor = "AssetBundle/Platform/Editor";
	const string Menu_PlatformAndroid = "AssetBundle/Platform/Android";
	const string Menu_PlatformiOS = "AssetBundle/Platform/iOS";

	[MenuItem("AssetBundle/Build", priority = 1)]
	static void Build()
	{
		string assetBundleDirectory = "Assets/AssetBundles/Unity";
		BuildTarget buildTarget = BuildTarget.StandaloneWindows;
		if (true == EditorPrefs.GetBool(Menu_PlatformAndroid))
		{
			assetBundleDirectory = "Assets/AssetBundles/Android";
			buildTarget = BuildTarget.Android;
		}
		else if (true == EditorPrefs.GetBool(Menu_PlatformiOS))
		{
			assetBundleDirectory = "Assets/AssetBundles/iOS";
			buildTarget = BuildTarget.iOS;
		}
			
		if (false == Directory.Exists(assetBundleDirectory))
		{
			Directory.CreateDirectory(assetBundleDirectory);
		}

		BuildPipeline.BuildAssetBundles(assetBundleDirectory, BuildAssetBundleOptions.None, buildTarget);
		AssetDatabase.Refresh();
	}

	[MenuItem(Menu_LocalServer)]
	static void LocalServer()
	{
		MenuToggle(Menu_LocalServer);
	}

	[MenuItem("AssetBundle/Platform/Editor")]
	static void PlatformEditor()
	{
		MenuSetBool(Menu_PlatformEditor, true);
		MenuSetBool(Menu_PlatformAndroid, false);
		MenuSetBool(Menu_PlatformiOS, false);
	}
	[MenuItem("AssetBundle/Platform/Android")]
	static void PlatformAndroid()
	{
		MenuSetBool(Menu_PlatformEditor, false);
		MenuSetBool(Menu_PlatformAndroid, true);
		MenuSetBool(Menu_PlatformiOS, false);
	}
	[MenuItem("AssetBundle/Platform/iOS")]
	static void PlatformiOS()
	{
		MenuSetBool(Menu_PlatformEditor, false);
		MenuSetBool(Menu_PlatformAndroid, false);
		MenuSetBool(Menu_PlatformiOS, true);
	}
	static bool MenuToggle(string menu)
	{
		bool enable = EditorPrefs.GetBool(menu, false);
		Menu.SetChecked(menu, !enable);
		EditorPrefs.SetBool(menu, !enable);
		return !enable;
	}

	static void MenuSetBool(string menu, bool enable)
	{
		Menu.SetChecked(menu, enable);
		EditorPrefs.SetBool(menu, enable);
	}
#endif
}
