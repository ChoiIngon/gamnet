using UnityEditor;
using System.IO;

public class BuildAssetBundles
{
	[MenuItem("Assets/Build AssetBundles")]
	static void Build()
	{
		string assetBundleDirectory = "Assets/AssetBundles/Unity/";
#if UNITY_ANDROID
		assetBundleDirectory = "Assets/AssetBundles/Android";
#endif
#if UNITY_IOS
		assetBundleDirectory = "Assets/AssetBundles/iOS";
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
