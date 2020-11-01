﻿using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;
using System.Collections;

class GameManager : Util.MonoSingleton<GameManager>
{
	private Network.LobbySession lobby_session = new Network.LobbySession();

	public bool run_in_background = true;
	public string asset_bundle_url = "";

	public Component.Lobby lobby = null;
	public Component.Dungeon dungeon = null;

	public Component.Counter counter = null;
	public Component.MailBox mailbox = null;
	public Component.Bag bag = null;

	public UI.UIMain ui = null;
	public Network.LobbySession LobbySession
	{
		get { return lobby_session; }
	}

	private void Awake()
	{
		DontDestroyOnLoad(transform.gameObject);
		if (true == Application.isEditor)
		{
			Application.runInBackground = run_in_background;
		}

		AssetBundleManager.Instance.url = asset_bundle_url;

		lobby		= transform.Find("Lobby").GetComponent<Component.Lobby>();
		dungeon		= transform.Find("Dungeon").GetComponent<Component.Dungeon>();

		counter		= new Component.Counter();
		mailbox		= new Component.MailBox();
		bag			= new Component.Bag();

		ui = GetComponent<UI.UIMain>();
	}

	private IEnumerator Start()
	{
		yield return Item.Manager.Instance.Init();
		lobby.gameObject.SetActive(true);
		ui.login.gameObject.SetActive(true);
	}

	private void Update()
	{
		if (null != lobby_session)
		{
			lobby_session.Update();
		}
	}
}

