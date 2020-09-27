#ifndef _DUNGEON_H
#define	_DUNGEON_H

#include <vector>
#include <set>
#include <memory>
#include "Rect.h"
#include "Tile.h"

class Player;
class Unit;

namespace Component {
class Dungeon
{
	struct Block
	{
		enum class Type
		{
			Invalid,
			Corridor,
			Room
		};
	
		Block(int id);
		void AddNeighbor(std::shared_ptr<Block> block);

		const int id;
		Type type;

		size_t distance;
		RectInt rect;
		std::vector<std::shared_ptr<Block>> neighbors;
		std::vector<int> doors;
	};

	struct Edge
	{
		int id;
		int parent;
		int distance;
		bool visit;
	};

	struct Path
	{
		Path(int prev, int tileIndex);
		int prev;
		int tile_index;
	};

public :
	Dungeon();

	void Init();
	std::shared_ptr<Tile> GetTile(int x, int y) const;
private :
	void InitBlocks();
	void InitTiles();
	void BuildPath();
	void BuildWall();
	void MoveToEmptySpace(std::shared_ptr<Block> block);
	bool OverlapWithExistBlocks(std::shared_ptr<Block> block);
	std::list<std::shared_ptr<Block>> FindNeighborBlocks(int blockID);
	std::shared_ptr<Block> FindBlock(int blockID);
	std::vector<int> GetIntersectArea(const RectInt& lhs, const RectInt& rhs);
	std::list<std::shared_ptr<Block>> FindPath(std::shared_ptr<Block> from, std::shared_ptr<Block> to);
	std::shared_ptr<std::list<std::shared_ptr<Dungeon::Block>>> FindPath(std::shared_ptr<Block> from, std::shared_ptr<Block> to, std::set<int>& visit);
	void FindDoor(std::shared_ptr<Block> block, std::shared_ptr<Block> neighbor);
	void BuildRoomInBlock(std::shared_ptr<Block> block);
	void BuildCorridorInBlock(std::shared_ptr<Block> block);
public :
	int room_count;
	int min_room_width;
	int max_room_width;
	int min_room_height;
	int max_room_height;
	int min_distance;

	RectInt rect;
	std::vector<std::shared_ptr<Block>> blocks;
	std::vector<std::shared_ptr<Tile>> tiles;
	std::shared_ptr<Block> start;
	std::shared_ptr<Block> end;

public :
	std::shared_ptr<Unit> player;
	std::shared_ptr<Unit> monster;
};

}
#endif
