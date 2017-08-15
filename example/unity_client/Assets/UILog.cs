using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UILog : MonoBehaviour {
	private static UILog _instance;  
	public static UILog Instance  
	{  
		get  
		{  
			if (!_instance) 
			{  
				_instance = (UILog)GameObject.FindObjectOfType(typeof(UILog));  
				if (!_instance)  
				{  
					GameObject container = new GameObject();  
					container.name = "UILog";  
					_instance = container.AddComponent<UILog>();  
				}  
				_instance.Init ();
				//DontDestroyOnLoad (_instance.gameObject);
			}  

			return _instance;  
		}  
	}	
	Text text;
	public int lineLimit;
	private int lineCount;
	void Init()
	{
		text = GetComponent<Text> ();
		lineCount = 0;
	}

	public void Write(string text)
	{
		this.text.text += text + System.Environment.NewLine;
		lineCount++;
		if (lineLimit < lineCount) {
			int index = this.text.text.IndexOf(System.Environment.NewLine);
			this.text.text = this.text.text.Substring(index + System.Environment.NewLine.Length);
		}
	}
}