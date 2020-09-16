using UnityEngine;

[System.Serializable]
public class Unit
{
	public float maxHP;
	public float currentHP;
	public float attack;
	public float defense;
	public float speed;
	public float critical;
	public int sight;

	public System.Action<float> onDamage;

	public void Attack(Unit target)
	{
		target.currentHP -= attack;
	}						   

	static public GameObject InitPartsSprite(Transform parent, string name, string spritePath, int sortingOrder)
	{
		GameObject obj = new GameObject();
		obj.name = name;
		obj.transform.SetParent(parent, false);
		SpriteRenderer spriteRenderer = obj.AddComponent<SpriteRenderer>();
		spriteRenderer.sprite = ResourceManager.Instance.Load<Sprite>(spritePath);
		spriteRenderer.sortingLayerName = "Unit";
		spriteRenderer.sortingOrder = sortingOrder;
		return obj;
	}
}
