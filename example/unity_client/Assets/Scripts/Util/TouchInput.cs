using UnityEngine;
using System;

[RequireComponent(typeof(BoxCollider2D))]
public class TouchInput : MonoBehaviour
{
	private Vector3 mouse_position;
	private int _block_count = 0;
	private BoxCollider2D _touch_collider;
	public Action<Vector3> on_touch_down;
    public Action<Vector3> on_touch_up;
    public Action<Vector3> on_touch_drag;
	public BoxCollider2D touch_collider
	{
		get
		{
			if (null == _touch_collider)
			{
				_touch_collider = GetComponent<BoxCollider2D>();
			}
			return _touch_collider;
		}
	}
	public int block_count
	{
		set
		{
			_block_count = value;
			if (0 > _block_count)
			{
				throw new System.Exception("under zero touch block count");
			}

			if (0 < _block_count)
			{
				touch_collider.enabled = false;
			}
			else
			{
				touch_collider.enabled = true;
			}
		}
		get
		{
			return _block_count;
		}
	}
	
    void Awake()
    {
		mouse_position = Input.mousePosition;
		_block_count = 0;
	}

	void OnMouseDown()
	{
		float distanceToScreen = Camera.main.WorldToScreenPoint(gameObject.transform.position).z;
		mouse_position = Camera.main.ScreenToWorldPoint (
			new Vector3(Input.mousePosition.x, Input.mousePosition.y, distanceToScreen)
		);
        on_touch_down?.Invoke(mouse_position);
    }

	void OnMouseUp()
	{
		on_touch_up?.Invoke(mouse_position);
	}

	void OnMouseDrag()
	{
		float distanceToScreen = Camera.main.WorldToScreenPoint(gameObject.transform.position).z;
		mouse_position = Camera.main.ScreenToWorldPoint(
			new Vector3(Input.mousePosition.x, Input.mousePosition.y, distanceToScreen)
		);

		on_touch_drag?.Invoke(mouse_position);
	}
}
