#ifndef _DUNGEON_H
#define	_DUNGEON_H

#include <vector>
#include <set>
#include <map>
#include <memory>
#include "Rect.h"
#include "Tile.h"
#include "TileMap.h"
#include "../../Util/MetaData.h"

class Player;
class Unit;

namespace Component { namespace Dungeon {
	class Data;
	struct Meta : public std::enable_shared_from_this<Meta>, public MetaData
	{
		struct Room : public MetaData
		{
			Room();
			int count;
			int min_width;
			int max_width;
			int min_height;
			int max_height;
			int min_distance;
		};

		struct Monster : public MetaData
		{
			Monster();
			std::string id;
			int level;
			int count;
		};

		Meta();
		std::string id;
		int			index;
		int			level;
		Room		room;
		std::vector<Monster> monsters;

		std::shared_ptr<Data> CreateInstance();

		virtual void OnLoad() override;
	};

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

	class Data
	{
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

	public:
		Data(const Meta& meta);

		void Init();
		std::shared_ptr<Tile> GetTile(int x, int y) const;
		std::shared_ptr<Tile> GetTile(const Vector2Int& coordinate) const;
	protected :
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
		std::shared_ptr<std::list<std::shared_ptr<Block>>> FindPath(std::shared_ptr<Block> from, std::shared_ptr<Block> to, std::set<int>& visit);
		void FindDoor(std::shared_ptr<Block> block, std::shared_ptr<Block> neighbor);
		void BuildRoomInBlock(std::shared_ptr<Block> block);
		void BuildCorridorInBlock(std::shared_ptr<Block> block);
	public:
		const Meta& meta;

		RectInt rect;
		std::vector<std::shared_ptr<Block>> blocks;
		std::vector<std::shared_ptr<Tile>> tiles;
		std::shared_ptr<Block> start;
		std::shared_ptr<Block> end;

	public:
		std::shared_ptr<Unit> player;
		std::map<uint64_t, std::shared_ptr<Unit>> monster;
	};

	class Data2 : public TileMap
	{
		struct Edge
		{
			std::shared_ptr<Block> block;
			std::shared_ptr<Edge> parent;
			int distance;
			bool visit;
		};
	public :
		Data2(const Meta& meta);
		void Init();

		const Meta& meta;
	private :
		std::list<std::shared_ptr<Block>> Data2::FindNeighborBlocks(std::shared_ptr<Block> block);
		void MoveToEmptySpace(std::shared_ptr<Block> block);
		bool OverlapWithExistBlocks(std::shared_ptr<Block> block);
		
		std::map<int, std::shared_ptr<Block>> blocks;
	};

	class Manager
	{
	public :
		void Init();

		std::shared_ptr<Meta> FindMeta(const std::string& id);
		std::shared_ptr<Meta> FindMeta(uint32_t index);
	private :
		std::map<uint32_t, std::shared_ptr<Meta>> index_metas;
		std::map<std::string, std::shared_ptr<Meta>> id_metas;
	};
};

}
#endif
