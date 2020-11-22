#ifndef _DUNGEON_H
#define	_DUNGEON_H

#include <vector>
#include <set>
#include <map>
#include <memory>
#include "Rect.h"
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

		const int id;
		Type type;

		size_t distance;
		RectInt rect;
	};

	class Data : public TileMap
	{
		struct Edge
		{
			std::shared_ptr<Block> block;
			std::shared_ptr<Edge> parent;
			int distance;
			bool visit;
		};
	public :
		Data(const Meta& meta);
		void Init();

		const Meta& meta;
		std::shared_ptr<Block> start;
		std::shared_ptr<Block> end;
		std::shared_ptr<Unit> player;
		std::map<uint64_t, std::shared_ptr<Unit>> monster;
	private :
		static constexpr int ROOM_COUNT_MULTIPLE = 3;
		std::list<std::shared_ptr<Block>> FindNeighborBlocks(std::shared_ptr<Block> block);
		std::list<Vector2Int> BuildPath();

		void MoveToEmptySpace(std::shared_ptr<Block> block);
		bool OverlapWithExistBlocks(std::shared_ptr<Block> block);
		void ShuffleBlocks();
	private :
		static const Vector2Int FOUR_DIRECTION[4];
		static const Vector2Int EIGHT_DIRECTION[8];
		std::map<int, std::shared_ptr<Block>> blocks;
		std::vector<std::shared_ptr<Block>> random_order_blocks;
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
