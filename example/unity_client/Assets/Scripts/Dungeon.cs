using System.Collections.Generic;
using UnityEngine;

namespace Component
{
	public class Dungeon : MonoBehaviour
	{
		public List<Tile> tiles = new List<Tile>();
		public int width = 0;
		public int height = 0;
		
		public void EnableTouch(bool flag)
		{
			foreach (Tile tile in tiles)
			{
				if (true == flag)
				{
					tile.touchInput.block_count--;
				}
				else
				{
					tile.touchInput.block_count++;
				}
			}
		}

		public Tile.Data GetTileData(int tileIndex)
		{
			if (0 > tileIndex || width * height <= tileIndex)
			{
				return null;
			}
			
			if (null == tiles[tileIndex])
			{
				return null;
			}
			return tiles[tileIndex].data;
		}

		public Tile.Data GetTileData(int x, int y)
		{
			return GetTileData(y * width + x);
		}

		void OnDrawGizmosSelected()
		{
			/*
			foreach (Block block in data.blocks)
			{
				Gizmos.DrawLine(new Vector3((float)block.rect.x - 0.5f, (float)block.rect.y - 0.5f, 0.0f), new Vector3((float)block.rect.x - 0.5f, (float)block.rect.yMax - 0.5f, 0.0f));
				Gizmos.DrawLine(new Vector3((float)block.rect.x - 0.5f, (float)block.rect.y - 0.5f, 0.0f), new Vector3((float)block.rect.xMax - 0.5f, (float)block.rect.y - 0.5f, 0.0f));
				Gizmos.DrawLine(new Vector3((float)block.rect.x - 0.5f, (float)block.rect.yMax - 0.5f, 0.0f), new Vector3((float)block.rect.xMax - 0.5f, (float)block.rect.yMax - 0.5f, 0.0f));
				Gizmos.DrawLine(new Vector3((float)block.rect.xMax - 0.5f, (float)block.rect.y - 0.5f, 0.0f), new Vector3((float)block.rect.xMax - 0.5f, (float)block.rect.yMax - 0.5f, 0.0f));
			}
			*/
		}
	}

}