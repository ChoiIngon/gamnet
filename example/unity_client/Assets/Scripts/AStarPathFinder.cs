using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class AStarPathFinder : IEnumerable
{
	public class Node
	{
		public Node(AStarPathFinder pathFinder, Node parent, Vector2Int from, int pathCost)
		{
			path_finder = pathFinder;
			position = from;
			expect_cost += Mathf.Abs(pathFinder.destination.x - from.x);
			expect_cost += Mathf.Abs(pathFinder.destination.y - from.y);
			path_cost = pathCost;
			index = from.y * path_finder.dungeon.width + from.x;
			this.parent = parent;
		}

		public void FindPath()
		{
			if (true == path_finder.open_nodes.ContainsKey(index))
			{
				path_finder.open_nodes.Remove(index);
			}

			if (true == path_finder.close_nodes.ContainsKey(index))
			{
				//Debug.Log("can't find path(position:" + position.ToString() + ")");
				return;
			}

			path_finder.close_nodes.Add(index, this);
			if (position == path_finder.destination)
			{
				Node trace = this;
				while (null != trace.parent)
				{
					path_finder.path.Insert(0, trace.position);
					trace = trace.parent;
				}
				path_finder.path.Insert(0, trace.position);
				return;
			}

			List<Node> children = new List<Node>();
			foreach (Vector2Int offset in LOOKUP_OFFSETS)
			{
				Vector2Int childPosition = new Vector2Int(position.x + offset.x, position.y + offset.y);
				if (0 > childPosition.x || 0 > childPosition.y || path_finder.dungeon.width <= childPosition.x || path_finder.dungeon.height <= childPosition.y)
				{
					continue;
				}

				Tile.Data tile = path_finder.dungeon.GetTileData(childPosition.x, childPosition.y);
				if (Message.DungeonTileType.Wall == tile.type)
				{
					//Debug.Log("not tile(child position:x" + childPosition.x + ", y:" + childPosition.y +")");
					continue;
				}

				Node childNode = new Node(path_finder, this, childPosition, path_cost + 1);
				if (true == path_finder.close_nodes.ContainsKey(childNode.index))
				{
					//Debug.Log ("already close node(child position:x" + childPosition.x + ", y:" + childPosition.y +")");
					continue;
				}
				if (true == path_finder.open_nodes.ContainsKey(childNode.index))
				{
					Node openNode = path_finder.open_nodes[childNode.index];
					if (openNode.path_cost < this.path_cost)
					{
						this.path_cost = openNode.path_cost + 1;
						this.parent = openNode;
						continue;
					}
				}
				else
				{
					path_finder.open_nodes.Add(childNode.index, childNode);
				}
				children.Add(childNode);
			}
			children.Sort(delegate (Node lhs, Node rhs) {
				return lhs.cost.CompareTo(rhs.cost);
			});
			foreach (Node child in children)
			{
				child.FindPath();
				if (0 < path_finder.path.Count)
				{
					return;
				}
			}
			//Debug.Log ("cant find destination(x:" + position.x + ", y:" + position.y + ")");
		}

		public int index = 0;
		public int path_cost = 0;
		public int expect_cost = 0;
		public int cost
		{
			get { return path_cost + expect_cost; }
		}
		public Node parent = null;
		public Vector2Int position;
		private AStarPathFinder path_finder = null;
		
		static Vector2Int[] LOOKUP_OFFSETS = {
			new Vector2Int(-1,  0),
			new Vector2Int( 1,  0),
			new Vector2Int( 0, -1),
			new Vector2Int( 0,  1)
		};
	}

	public Component.Dungeon dungeon;
	public Dictionary<int, Node> open_nodes = new Dictionary<int, Node>();
	public Dictionary<int, Node> close_nodes = new Dictionary<int, Node>();
	public Vector2Int destination = Vector2Int.zero;
	public List<Vector2Int> path = new List<Vector2Int>();

	public AStarPathFinder(Component.Dungeon dungeon, Vector2Int from, Vector2Int to)
	{
		this.dungeon = dungeon;
		destination = to;
		Node node = new Node(this, null, from, 0);
		node.FindPath();		
	}

	public IEnumerator GetEnumerator()
	{
		if (0 == path.Count)
		{
			yield break;
		}
		Vector2Int position = path[0];
		path.RemoveAt(0);
		yield return position;
	}
}
