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
	public static void Write(string text)
	{
		UILog.Instance._Write (text);
	}

	private Text text;
	private int lineCount;
	private int lineNum;
	public int lineLimit;

	void Init()
	{
		text = GetComponent<Text> ();
		lineCount = 0;
		lineNum = 1;
	}

	void _Write(string text)
	{
		this.text.text += lineNum.ToString() + ":" + text + System.Environment.NewLine;
		lineCount++;
		lineNum++;
		if (lineLimit < lineCount) {
			int index = this.text.text.IndexOf(System.Environment.NewLine);
			this.text.text = this.text.text.Substring(index + System.Environment.NewLine.Length);
		}
	}
}