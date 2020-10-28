﻿using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;

class GameManager : Util.MonoSingleton<GameManager>
{
	public bool run_in_background = true;

	private Network.LobbySession lobby_session = new Network.LobbySession();
	public Network.LobbySession LobbySession
	{
		get { return lobby_session; }
	}

	public UI.UIMain ui;
	private void Awake()
	{
		DontDestroyOnLoad(transform.gameObject);
		if (true == Application.isEditor)
		{
			Application.runInBackground = run_in_background;
		}
	}

	private void Start()
	{
		if (true == Application.isEditor)
		{
			Application.runInBackground = run_in_background;
		}
		
		ui = GetComponent<UI.UIMain>();

		ui.SetActive(UI.UIMain.UIKey_Login, true);
	}

	private void Update()
	{
		if (null != lobby_session)
		{
			lobby_session.Update();
		}
	}
}

