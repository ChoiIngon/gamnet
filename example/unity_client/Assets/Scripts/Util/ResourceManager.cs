using UnityEngine;
using System.Collections.Generic;

public class ResourceManager : Util.Singleton<ResourceManager>
{
	Dictionary<string, Object> resource = new Dictionary<string, Object>();

	private void Start()
	{
	}

	public T Load<T>(string path) where T : Object
	{
		if (true == resource.ContainsKey(path))
		{
			return resource[path] as T;
		}

		T asset = Resources.Load<T>(path);
		resource[path] = asset ?? throw new System.Exception("can not find asset(path:" + path + ")");
		return asset;
	}
}
