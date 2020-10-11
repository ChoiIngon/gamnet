using UnityEditor;
using System.IO;

public class BuildAssetBundles
{
	[MenuItem("Assets/Build AssetBundles")]
	static void Build()
	{
#if UNITY_EDITOR
		string assetBundleDirectory = "Assets/AssetBundles/Unity/";
#endif
#if UNITY_ANDROID
		string assetBundleDirectory = "Assets/AssetBundles/Android";
#endif
#if UNITY_IOS
		string assetBundleDirectory = "Assets/AssetBundles/iOS";
#endif
		if (false == Directory.Exists(assetBundleDirectory))
		{
			Directory.CreateDirectory(assetBundleDirectory);
		}
#if UNITY_EDITOR
		BuildTarget buildTarget = BuildTarget.StandaloneWindows;
#endif
		BuildPipeline.BuildAssetBundles(assetBundleDirectory, BuildAssetBundleOptions.None, buildTarget);
		AssetDatabase.Refresh();
	}
}
