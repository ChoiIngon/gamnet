using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Gamnet {
public class SyncQueue<T> {
	private object _sync_obj = new object();
	private List<T> items = new List<T>();

	public int Count() {
		lock (_sync_obj) {
			return items.Count;
		}
	}

	public void PushBack(T item) {
		lock (_sync_obj) {
			items.Add(item);
		}
	}

	public void PushFront(T item) {
		lock (_sync_obj) {
			items.Insert(0, item);
		}
	}

	public T PopFront() {
		lock (_sync_obj) {
			T item = items[0];
			items.RemoveAt(0);
			return item;
		}
	}

	public void RemoveAt(int index) {
		lock (_sync_obj) {
			items.RemoveAt(index);
		}
	}

	public T this[int index] {
		get {
			lock (_sync_obj) {
				return items [index];
			}
		}
	}

	public void Clear() {
		lock (_sync_obj) {
			items.Clear();
		}
	}

	public List<T> Copy() {
		lock (_sync_obj) {
			List<T> copy = new List<T>(items);
			return copy;
		}
	}
}
}