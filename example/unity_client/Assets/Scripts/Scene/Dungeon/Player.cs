using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
	public SpriteRenderer trunk;
	public SpriteRenderer hair;
	public SpriteRenderer beard;
	public SpriteRenderer [] suit = new SpriteRenderer[(int)Message.EquipItemPartType.Max];
	public Unit unit;	
	// Start is called before the first frame update
	private void Start()
	{
		unit = GetComponent<Unit>();
		trunk.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(Component.Suit.DEFAULT_BASE_IMAGE);
		hair.sprite = AssetBundleManager.Instance.LoadAsset<Sprite>(Component.Suit.DEFAULT_HAIR_IMAGE);
	}

	public void OnEquip(Message.EquipItemPartType part, Sprite sprite)
	{
		SpriteRenderer spriteRenderer = suit[(int)part];
		spriteRenderer.sprite = sprite; 
		spriteRenderer.gameObject.SetActive(true);
	}

	public void OnUnequip(Message.EquipItemPartType part)
	{
		suit[(int)part].gameObject.SetActive(false);
	}
}