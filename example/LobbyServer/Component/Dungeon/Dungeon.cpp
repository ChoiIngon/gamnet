#include "Dungeon.h"
#include "Unit.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include <iostream>
#include "AStarPathFinder.h"

namespace Component { namespace Dungeon {

	Meta::Room::Room()
		: count(0)
		, min_width(0)
		, max_width(0)
		, min_height(0)
		, max_height(0)
		, min_distance(0)
	{
		META_MEMBER(count);
		META_MEMBER(min_width);
		META_MEMBER(max_width);
		META_MEMBER(min_height);
		META_MEMBER(max_height);
		META_MEMBER(min_distance);
	}

	Meta::Monster::Monster()
		: id("")
		, level(0)
		, count(0)
	{
		META_MEMBER(id);
		META_MEMBER(level);
		META_MEMBER(count);
	}

	Meta::Meta()
		: id("")
		, index(0)
		, level(0)
	{
		META_MEMBER(id);
		META_MEMBER(index);
		META_MEMBER(level);
		META_MEMBER(room);
		META_MEMBER(monsters);
	}

	Block::Block(int id)
		: id(id)
		, type(Type::Invalid)
		, distance(0)
	{
	}
	
	void Block::AddNeighbor(std::shared_ptr<Block> block)
	{
		auto itr = std::find_if(neighbors.begin(), neighbors.end(), [block](const std::shared_ptr<Block>& neighbor) { return neighbor->id == block->id; });
		if(itr == neighbors.end())
		{
			neighbors.push_back(block);
		}
	}

	Data::Path::Path(int prev, int tileIndex)
		: prev(prev)
		, tile_index(tileIndex)
	{
	}

	Data::Data(const Meta& meta)
		: meta(meta)
		, start(nullptr)
		, end(nullptr)
	{
	}

	void Data::Init()
	{
		InitBlocks();
		InitTiles();
		BuildPath();
		BuildWall();

		start = blocks[0];
		end = blocks[0];

		for(auto block : blocks)
		{
			if (Block::Type::Room == block->type)
			{
				if (start->distance < block->distance)
				{
					start = block;
				}
			}
		}

		for(auto block : blocks)
		{
			if (Block::Type::Room == block->type)
			{
				if (start->id == block->id)
				{
					continue;
				}

				std::list<std::shared_ptr<Block>> path = FindPath(start, block);
				if (end->distance < path.size())
				{
					end->distance = path.size();
					end = block;
				}
			}
		}

		tiles[(int)start->rect.Center().y * rect.width + (int)start->rect.Center().x]->type = Tile::Type::StairUp;
		tiles[(int)end->rect.Center().y * rect.width + (int)end->rect.Center().x]->type = Tile::Type::StairDown;
	}

	std::shared_ptr<Tile> Data::GetTile(int x, int y) const
	{
		if (rect.x > x || rect.y > y || rect.xMax <= x || rect.yMax <= y)
		{
			return nullptr;
		}
		return tiles[y * rect.width + x];
	}

	std::shared_ptr<Tile> Data::GetTile(const Vector2Int& coordinate) const
	{
		return GetTile(coordinate.x, coordinate.y);
	}

	void Data::InitBlocks()
	{
		int blockID = 1;

		for(int i=0; i<meta.room.count; i++)
		{
			std::shared_ptr<Block> block = std::make_shared<Block>(blockID++);
			block->type = Block::Type::Room;
			block->rect.width = Gamnet::Random::Range(meta.room.min_width, meta.room.max_width);
			block->rect.height = Gamnet::Random::Range(meta.room.min_height, meta.room.max_height);

			MoveToEmptySpace(block);

			int xMax = rect.xMax;
			int yMax = rect.yMax;
			rect.x = std::min(rect.x, block->rect.x);
			rect.y = std::min(rect.y, block->rect.y);
			rect.xMax = std::max(xMax, (int)block->rect.xMax);
			rect.yMax = std::max(yMax, (int)block->rect.yMax);
			blocks.push_back(block);
		}

		for(auto block : blocks)
		{
			block->rect.x -= rect.x;
			block->rect.y -= rect.y;
		}

		rect.x = 0;
		rect.y = 0;

		for(int y=0; y<rect.yMax; y++)
		{
			for(int x=0; x<rect.xMax; x++)
			{
				std::shared_ptr<Block> block = std::make_shared<Block>(blockID++);
				block->type = Block::Type::Corridor;
				block->rect.x = x;
				block->rect.y = y;
				block->rect.width = 1;
				block->rect.height = 1;

				int width = Gamnet::Random::Range(meta.room.min_width, meta.room.max_width);
				int height = Gamnet::Random::Range(meta.room.min_height, meta.room.max_height);

				if(false == OverlapWithExistBlocks(block))
				{
					if(0 == Gamnet::Random::Range(0, 1))
					{
						while(height > block->rect.height && (int)rect.yMax > (int)block->rect.yMax)
						{
							block->rect.yMax = block->rect.yMax + 1;
							if(true == OverlapWithExistBlocks(block))
							{
								block->rect.yMax = block->rect.yMax - 1;
								break;
							}
						}

						while (width > block->rect.width && (int)rect.xMax > (int)block->rect.xMax)
						{
							block->rect.xMax = block->rect.xMax + 1;
							if (true == OverlapWithExistBlocks(block))
							{
								block->rect.xMax = block->rect.xMax - 1;
								break;
							}
						}
					}
					else
					{
						while (width > block->rect.width && (int)rect.xMax > (int)block->rect.xMax)
						{
							block->rect.xMax = block->rect.xMax + 1;
							if (true == OverlapWithExistBlocks(block))
							{
								block->rect.xMax = block->rect.xMax - 1;
								break;
							}
						}

						while (height > block->rect.height && (int)rect.yMax > (int)block->rect.yMax)
						{
							block->rect.yMax = block->rect.yMax + 1;
							if (true == OverlapWithExistBlocks(block))
							{
								block->rect.yMax = block->rect.yMax - 1;
								break;
							}
						}
					}

					/*
					if (0 == block->id % 3 && min_room_width <= block->rect.width && min_room_height <= block->rect.height)
					{
						block->type = Block::Type::Room;
					}
					*/
					blocks.push_back(block);
				}
			}
		}

		for(auto block : blocks)
		{
			block->rect.x += 1;
			block->rect.y += 1;
		}
		rect.width += 2;
		rect.height += 2;
	}

	void Data::InitTiles()
	{
		tiles.resize((size_t)rect.width * rect.height);

		for (int i = 0; i < (int)tiles.size(); i++)
		{
			std::shared_ptr<Tile> tile = std::make_shared<Tile>(i);
			tile->position = Vector2Int(tile->index % rect.width, tile->index / rect.width);
			tiles[i] = tile;
		}

		for(auto block : blocks)
		{
			for (int x = block->rect.x; x < block->rect.xMax; x++)
			{
				tiles[block->rect.y * rect.width + x]->type = Tile::Type::Wall;
				tiles[(block->rect.yMax - 1) * rect.width + x]->type = Tile::Type::Wall;
			}

			for (int y = block->rect.y; y < block->rect.yMax; y++)
			{
				tiles[y * rect.width + block->rect.x]->type = Tile::Type::Wall;
				tiles[y * rect.width + block->rect.xMax - 1]->type = Tile::Type::Wall;
			}

			if (Block::Type::Room == block->type)
			{
				tiles[block->rect.y * rect.width + block->rect.x]->type = Tile::Type::Invalid;
				tiles[block->rect.y * rect.width + block->rect.xMax - 1]->type = Tile::Type::Invalid;
				tiles[(block->rect.yMax - 1) * rect.width + block->rect.x]->type = Tile::Type::Invalid;
				tiles[(block->rect.yMax - 1) * rect.width + block->rect.xMax - 1]->type = Tile::Type::Invalid;
			}
		}
	}

	void Data::BuildPath()
	{
		std::vector<std::shared_ptr<Edge>> edges;
		for(auto block : blocks)
		{
			std::shared_ptr<Edge> edge = std::make_shared<Edge>();
			edge->id = block->id;
			edge->parent = 0;
			edge->distance = std::numeric_limits<int>::max();
			edge->visit = false;
			edges.push_back(edge);
		}

		std::shared_ptr<Edge> edge = edges[0];
		edge->distance = 0;

		while (true)
		{
			std::sort(edges.begin(), edges.end(), [] (const std::shared_ptr<Edge>& lhs, const std::shared_ptr<Edge>& rhs)
			{
				return lhs->distance < rhs->distance;
			});

			bool finish = true;
			int visitCount = 0;
			for (int i = 0; i < (int)edges.size() - visitCount;)
			{
				std::shared_ptr<Edge> e = edges[i];
				if (true == e->visit)
				{
					edges.erase(edges.begin() + i);
					edges.push_back(e);
					visitCount++;
				}
				else
				{
					i++;
					finish = false;
				}
			}

			if (true == finish)
			{
				break;
			}

			edge = edges[0];
			edge->visit = true;
			std::list<std::shared_ptr<Block>> neighbors = FindNeighborBlocks(edge->id);
			for(auto neighbor : neighbors)
			{
				auto itr = std::find_if(edges.begin(), edges.end(), [neighbor](const std::shared_ptr<Edge>& e) { return e->id == neighbor->id; });
				std::shared_ptr<Edge> next = *itr;
				int distance = std::min(next->distance, edge->distance + Gamnet::Random::Range(1, 1000));
				if (distance < next->distance)
				{
					next->parent = edge->id;
					next->distance = distance;
				}
			}
		}

		for(auto e : edges)
		{
			std::shared_ptr<Block> parent = FindBlock(e->parent);
			if (nullptr == parent)
			{
				continue;
			}
			std::shared_ptr<Block> child = FindBlock(e->id);
			parent->AddNeighbor(child);
			child->AddNeighbor(parent);
		}

		std::shared_ptr<Block> start = FindBlock(1);
		for(auto block : blocks)
		{
			if (start->id == block->id)
			{
				continue;
			}

			if (Block::Type::Corridor == block->type)
			{
				continue;
			}

			std::list<std::shared_ptr<Block>> path = FindPath(start, block);
			std::shared_ptr<Block> from = start;
			int distance = 1;
			while (0 < path.size())
			{
				std::shared_ptr<Block> to = path.front();
				to->distance = distance++;
				FindDoor(from, to);
				from = path.front();
				path.pop_front();
			}
		}
	}

	std::list<std::shared_ptr<Block>> Data::FindPath(std::shared_ptr<Block> from, std::shared_ptr<Block> to)
	{
		std::set<int> vistedTileIndices;
		auto path = FindPath(from, to, vistedTileIndices);
		if(nullptr == path)
		{
			return std::list<std::shared_ptr<Block>>();
		}
		return *path;
	}

	std::shared_ptr<std::list<std::shared_ptr<Block>>> Data::FindPath(std::shared_ptr<Block> from, std::shared_ptr<Block> to, std::set<int>& visit)
	{
		if (from->id == to->id)
		{
			return std::make_shared<std::list<std::shared_ptr<Block>>>();
		}

		visit.insert(from->id);

		for(auto neighbor : from->neighbors)
		{
			if (visit.end() != visit.find(neighbor->id))
			{
				continue;
			}
			std::shared_ptr<std::list<std::shared_ptr<Block>>> path = FindPath(neighbor, to, visit);
			if (nullptr != path)
			{
				path->push_front(neighbor);
				return path;
			}
		}

		return nullptr;
	}

	void Data::FindDoor(std::shared_ptr<Block> block, std::shared_ptr<Block> neighbor)
	{
		std::vector<RectInt> roomOffsets = {
			RectInt( 1,  1,  0, -2),    // right, ignore top, bottom tiles
			RectInt( 1, -1, -2,  0),    // down, ignore left, right tiles
			RectInt(-1,  1,  0, -2),    // left, ignore top, bottom tiels
			RectInt( 1,  1, -2,  0)     // up, ignore left, right tiles
		};

		std::vector<RectInt> corridorOffsets = {
			RectInt( 1,  0,  0,  0),	// right, ignore top, bottom tiles
			RectInt( 0, -1,  0,  0),	// down, ignore left, right tiles
			RectInt(-1,  0,  0,  0),	// left, ignore top, bottom tiels
			RectInt( 0,  1,  0,  0)		// up, ignore left, right tiles
		};

		std::vector<RectInt> offsets = roomOffsets;
		if (Block::Type::Corridor == block->type)
		{
			offsets = corridorOffsets;
		}

		std::vector<int> doorIndiceOfNeighbor = {
			-1,
			rect.width,
			1,
			-rect.width
		};

		for (int i = 0; i < (int)offsets.size(); i++)
		{
			RectInt offset = offsets[i];
			RectInt blockRect(block->rect.x + offset.x, block->rect.y + offset.y, block->rect.width + offset.width, block->rect.height + offset.height);
			std::vector<int> overlapPoints = GetIntersectArea(blockRect, neighbor->rect);
			if (0 < overlapPoints.size())
			{
				for(int overlapPoint : overlapPoints)
				{
					if (Tile::Type::Door == tiles[overlapPoint]->type)
					{
						return;
					}
				}

				int door = overlapPoints[Gamnet::Random::Range(0, (int32_t)overlapPoints.size() - 1)];
				tiles[door]->type = Tile::Type::Door;
				tiles[door + doorIndiceOfNeighbor[i]]->type = Tile::Type::Door;
				block->doors.push_back(door + doorIndiceOfNeighbor[i]);
				neighbor->doors.push_back(door);
				return;
			}
		}
	}

	void Data::MoveToEmptySpace(std::shared_ptr<Block> block)
	{
		float x = Gamnet::Random::Range(1.0f, (float)block->rect.width - 1);
		float y = Gamnet::Random::Range(1.0f, (float)block->rect.height - 1);

		int dx = (0 == Gamnet::Random::Range(0, 1) ? 1 : -1);
		int dy = (0 == Gamnet::Random::Range(0, 1) ? 1 : -1);
		
		while (true)
		{
			if (false == OverlapWithExistBlocks(block))
			{
				return;
			}

			if (1.0f > y / x)
			{
				block->rect.x += dx * meta.room.min_distance;
				block->rect.y = dy * (int)(y / x * block->rect.x) + Gamnet::Random::Range(0, 3);
			}
			else
			{
				block->rect.y += dy * meta.room.min_distance;
				block->rect.x = dx * (int)(x / y * block->rect.y) + Gamnet::Random::Range(0, 3);
			}
		}
	}

	bool Data::OverlapWithExistBlocks(std::shared_ptr<Block> block)
	{
		for(std::shared_ptr<Block> exist : blocks)
		{
			if (true == exist->rect.Overlaps(block->rect))
			{
				return true;
			}
		}
		return false;
	}

	std::shared_ptr<Block> Data::FindBlock(int blockID)
	{
		for(auto block : blocks)
		{
			if(blockID == block->id)
			{
				return block;
			}
		}
		return nullptr;
	}

	std::list<std::shared_ptr<Block>> Data::FindNeighborBlocks(int blockID)
	{
		std::list<std::shared_ptr<Block>> neighbors;
   
		std::vector<RectInt> roomOffsets = {
			RectInt( 1,  1,  0, -2),	// right, ignore top, bottom tiles
			RectInt( 1, -1, -2,  0),	// down, ignore left, right tiles
			RectInt(-1,  1,  0, -2),	// left, ignore top, bottom tiels
			RectInt( 1,  1, -2,  0)		// up, ignore left, right tiles
		};

		std::vector<RectInt> corridorOffsets = {
			RectInt( 1,  0,  0,  0),	// right, ignore top, bottom tiles
			RectInt( 0, -1,  0,  0),	// down, ignore left, right tiles
			RectInt(-1,  0,  0,  0),	// left, ignore top, bottom tiels
			RectInt( 0,  1,  0,  0)		// up, ignore left, right tiles
		};

		std::shared_ptr<Block> block = FindBlock(blockID);
		std::vector<RectInt> offsets = roomOffsets;
		if (Block::Type::Corridor == block->type)
		{
			offsets = corridorOffsets;
		}

		for(auto neighbor : blocks)
		{
			if (block->id == neighbor->id)
			{
				continue;
			}

			for (int i = 0; i < (int)offsets.size(); i++)
			{
				const RectInt& offset = offsets[i];
				RectInt blockRect(block->rect.x + offset.x, block->rect.y + offset.y, block->rect.width + offset.width, block->rect.height + offset.height);
				if (false == blockRect.Intersect(neighbor->rect).Empyt())
				{
					neighbors.push_back(neighbor);
				}
			}
		}
		return neighbors;
	}

	std::vector<int> Data::GetIntersectArea(const RectInt& lhs, const RectInt& rhs)
	{
		std::vector<int> overlapPoints;

		if(false == lhs.Overlaps(rhs))
		{
			return overlapPoints;
		}

		RectInt intersection;
		intersection.x = std::max(lhs.x, rhs.x);
		intersection.y = std::max(lhs.y, rhs.y);
		intersection.xMax = std::min((int)lhs.xMax, (int)rhs.xMax);
		intersection.yMax = std::min((int)lhs.yMax, (int)rhs.yMax);

		for (int x = intersection.x; x < intersection.xMax; x++)
		{
			for (int y = intersection.y; y < intersection.yMax; y++)
			{
				if (Tile::Type::Invalid != tiles[y * rect.width + x]->type)
				{
					overlapPoints.push_back(y * rect.width + x);
				}
			}
		}
		return overlapPoints;
	}

	void Data::BuildWall()
	{
		for(auto tile : tiles)
		{
			tile->type = Tile::Type::Invalid;
		}

		for(auto block : blocks)
		{
			if (Block::Type::Room == block->type)
			{
				BuildRoomInBlock(block);
			}

			if (Block::Type::Corridor == block->type)
			{
				BuildCorridorInBlock(block);
			}
		}
	}

	void Data::BuildRoomInBlock(std::shared_ptr<Block> block)
	{
		for (int x = block->rect.x; x < block->rect.xMax; x++)
		{
			tiles[block->rect.y * rect.width + x]->type = Tile::Type::Wall;
			tiles[(block->rect.yMax - 1) * rect.width + x]->type = Tile::Type::Wall;
		}

		for (int y = block->rect.y; y < block->rect.yMax; y++)
		{
			tiles[y * rect.width + block->rect.x]->type = Tile::Type::Wall;
			tiles[y * rect.width + block->rect.xMax - 1]->type = Tile::Type::Wall;
		}

		for (int y = block->rect.y + 1; y < block->rect.yMax - 1; y++)
		{
			for (int x = block->rect.x + 1; x < block->rect.xMax - 1; x++)
			{
				tiles[y * rect.width + x]->type = Tile::Type::Floor;
			}
		}

		for(int door : block->doors)
		{
			tiles[door]->type = Tile::Type::Door;
		}
	}

	void Data::BuildCorridorInBlock(std::shared_ptr<Block> block)
	{
		std::vector<Vector2Int> offsets = {
			Vector2Int(1,  0),
			Vector2Int(0, -1),
			Vector2Int(-1,  0),
			Vector2Int(0,  1)
		};

		std::list<int> corridor;
		if (2 <= block->doors.size())
		{
			int start = block->doors[0];

			for (size_t i = 1; i < block->doors.size(); i++)
			{
				int dest = block->doors[i];

				std::vector<std::shared_ptr<Path>> queue;
				std::set<int> visit;
			
				queue.push_back(std::make_shared<Path>(0, start));
				visit.insert(start);

				int queueIndex = 0;
				while (queueIndex < (int)queue.size())
				{
					std::shared_ptr<Path> path = queue[queueIndex++];
					if (dest == path->tile_index)
					{
						break;
					}

					int x = path->tile_index % rect.width;
					int y = path->tile_index / rect.width;

					for(Vector2Int offset : offsets)
					{
						int dx = x + offset.x;
						int dy = y + offset.y;
						if (visit.end() == visit.find(dy * rect.width + dx) && block->rect.x <= dx && dx < block->rect.xMax && block->rect.y <= dy && dy < block->rect.yMax)
						{
							queue.push_back(std::make_shared<Path>(path->tile_index, dy * rect.width + dx ));
							visit.insert(dy * rect.width + dx);
						}
					}
				}

				int prev = dest;
				for (int j = (int)queue.size() - 1; j >= 0; j--)
				{
					std::shared_ptr<Path> path = queue[j];
					if (prev == path->tile_index)
					{
						corridor.push_back(path->tile_index);
						prev = path->prev;
					}
				}
			}
		}

		std::vector<int> surroundTileOffsets = {
			rect.width - 1,
			rect.width,
			rect.width + 1,
			1,
			-(rect.width + 1),
			-rect.width,
			-(rect.width - 1),
			-1
		};

		for(int tileIndex : corridor)
		{
			tiles[tileIndex]->type = Tile::Type::Floor;

			for (int i = 0; i < (int)surroundTileOffsets.size(); i++)
			{
				int surroundTileIndex = tileIndex + surroundTileOffsets[i];
				if (0 <= surroundTileIndex && surroundTileIndex < rect.width * rect.height)
				{
					if (Tile::Type::Invalid == tiles[surroundTileIndex]->type)
					{
						tiles[surroundTileIndex]->type = Tile::Type::Wall;
					}
				}
			}
		}
	}

	void Manager::Init()
	{
		MetaReader<Meta> reader;
		auto& rows = reader.Read("../MetaData/Dungeon.csv");
		for (auto& row : rows)
		{
			if (false == id_metas.insert(std::make_pair(row->id, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item id(", row->id, ")");
			}
			if (false == index_metas.insert(std::make_pair(row->index, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item index(", row->index, ")");
			}
		}
	}

	GAMNET_BIND_INIT_HANDLER(Dungeon::Manager, Init);

	std::shared_ptr<Data> Meta::CreateInstance()
	{
		return std::make_shared<Data>(*this);
	}

	void Meta::OnLoad()
	{
		room.count = std::max(room.count, 1);
		room.min_width = std::max(room.min_width, 3);
		room.max_width = std::max(room.max_width, 3);
		room.min_height = std::max(room.min_height, 3);
		room.max_height = std::max(room.min_height, 3);
		room.min_distance = std::max(room.min_distance, 1);
	}

	std::shared_ptr<Meta> Manager::FindMeta(const std::string& id)
	{
		auto itr = id_metas.find(id);
		if (id_metas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	std::shared_ptr<Meta> Manager::FindMeta(uint32_t index)
	{
		auto itr = index_metas.find(index);
		if (index_metas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	Data2::Data2(const Meta& meta) 
		: meta(meta)
	{
	}

	void Data2::Init()
	{
		blocks.clear();
		RectInt rect;

		rect.x = 0;
		rect.y = 0;
		rect.width = 0;
		rect.height = 0;
		std::vector<std::shared_ptr<Block>> shuffledBlocks;
		for(int i=0; i<meta.room.count * 3; i++)
		{
			std::shared_ptr<Block> block = std::make_shared<Block>(i+1);
			block->rect.x = 0;//(int)rect.Center().x;
			block->rect.y = 0;//(int)rect.Center().y;
			block->rect.width = Gamnet::Random::Range(meta.room.min_width, meta.room.max_width);
			block->rect.height = Gamnet::Random::Range(meta.room.min_height, meta.room.max_height);
			MoveToEmptySpace(block);

			int xMax = rect.xMax;
			int yMax = rect.yMax;
			rect.x = std::min(rect.x, block->rect.x);
			rect.y = std::min(rect.y, block->rect.y);
			rect.xMax = std::max(xMax, (int)block->rect.xMax);
			rect.yMax = std::max(yMax, (int)block->rect.yMax);
			shuffledBlocks.push_back(block);
			blocks.insert(std::make_pair(block->id, block));
		}

		//for(auto& itr : blocks)
		//{
		//	std::shared_ptr<Block> block = itr.second;
		//	std::list<std::shared_ptr<Block>> neighbors = FindNeighborBlocks(block);
		//	for(std::shared_ptr<Block> neighbor : neighbors)
		//	{
		//		neighbor->AddNeighbor(block);
		//		block->AddNeighbor(neighbor);
		//	}
		//}

		for (const auto& itr : blocks)
		{
			std::shared_ptr<Block> block = itr.second;
			block->rect.x -= rect.x;
			block->rect.y -= rect.y;
		}

		rect.x = 0;
		rect.y = 0;
		
		SetSize(rect.width, rect.height);
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(shuffledBlocks.begin(), shuffledBlocks.end(), g);

		for (int i=0; i<shuffledBlocks.size(); i++)
		{
			std::shared_ptr<Block> block = shuffledBlocks[i];
			block->type = Block::Type::Corridor;
			if(0 == i%3)
			{
				block->type = Block::Type::Room;
				GetTile(block->rect.x, block->rect.y)->type = Message::DungeonTileType::Wall;
				GetTile(block->rect.xMax - 1, block->rect.y)->type = Message::DungeonTileType::Wall;
				GetTile(block->rect.x, block->rect.yMax - 1)->type = Message::DungeonTileType::Wall;
				GetTile(block->rect.xMax - 1, block->rect.yMax - 1)->type = Message::DungeonTileType::Wall;
			}
		}
		
		////////////////////////다익스트라 패스//////////////////////////////////
		

		std::vector<std::shared_ptr<Edge>> edges;
		for (auto block : shuffledBlocks)
		{
			std::shared_ptr<Edge> edge = std::make_shared<Edge>();
			edge->block = block;
			edge->parent = nullptr;
			edge->distance = std::numeric_limits<int>::max();
			edge->visit = false;
			edges.push_back(edge);
		}

		std::shared_ptr<Edge> edge = edges[0];
		edge->distance = 0;

		while (true)
		{
			std::sort(edges.begin(), edges.end(), [](const std::shared_ptr<Edge>& lhs, const std::shared_ptr<Edge>& rhs)
			{
				return lhs->distance < rhs->distance;
			});

			bool finish = true;
			int visitCount = 0;
			for (int i = 0; i < (int)edges.size() - visitCount;)
			{
				std::shared_ptr<Edge> e = edges[i];
				if (true == e->visit)
				{
					edges.erase(edges.begin() + i);
					edges.push_back(e);
					visitCount++;
				}
				else
				{
					i++;
					finish = false;
				}
			}

			if (true == finish)
			{
				break;
			}

			edge = edges[0];
			edge->visit = true;
			std::list<std::shared_ptr<Block>> neighbors = FindNeighborBlocks(edge->block);
			for (std::shared_ptr<Block> neighbor : neighbors)
			{
				auto itr = std::find_if(edges.begin(), edges.end(), [neighbor](const std::shared_ptr<Edge>& e) { return e->block == neighbor; });

				std::shared_ptr<Edge> next = *itr;
				int distance = std::min(next->distance, edge->distance + Gamnet::Random::Range(1, 1000));
				if (distance < next->distance)
				{
					next->parent = edge;
					next->distance = distance;
				}
			}
		}
		
		//////////////////// path finding //////////////////////////////////////
		std::list<Vector2Int> paths;
		for(std::shared_ptr<Edge> edge : edges)
		{
			if(nullptr == edge->parent)
			{
				continue;
			}

			if(Block::Type::Room != edge->block->type)
			{
				continue;
			}

			std::shared_ptr<Edge> itr = edge;
			while(nullptr != itr->parent)
			{
				AStarPathFinder pathFinder(*this, itr->block->rect.Center(), itr->parent->block->rect.Center());
				paths.insert(paths.end(), pathFinder.path.begin(), pathFinder.path.end());
				itr = itr->parent;
			}
		}
		
		
		for (const Vector2Int& path : paths)
		{
			std::shared_ptr<TileMap::Tile> tile = GetTile(path.x, path.y);
			tile->type = Message::DungeonTileType::Floor;
		}
		
		for (const auto& itr : blocks)
		{
			std::shared_ptr<Block> block = itr.second;
			if(Block::Type::Room != block->type)
			{
				continue;
			}
			// 좌측 한쪽 벽에 문두개 연속으로 뚫는 부분 땜빵
			for (int y = block->rect.y; y < block->rect.yMax - 1; y++)
			{
				std::shared_ptr<TileMap::Tile> tile1 = GetTile(block->rect.x, y);
				std::shared_ptr<TileMap::Tile> tile2 = GetTile(block->rect.x, y + 1);
				if(Message::DungeonTileType::Floor == tile1->type && Message::DungeonTileType::Floor == tile2->type)
				{
					if(Message::DungeonTileType::Floor == GetTile(block->rect.x + 1, y)->type)
					{
						GetTile(block->rect.x, y)->type = Message::DungeonTileType::Invalid;
						
					}
					else //(Message::DungeonTileType::Floor == GetTile(block->rect.x + 1, y + 1)->type)
					{
						GetTile(block->rect.x, y + 1)->type = Message::DungeonTileType::Invalid;
					}
				}
			}
			// 우측 한쪽 벽에 문두개 연속으로 뚫는 부분 땜빵
			for (int y = block->rect.y; y < block->rect.yMax - 1; y++)
			{
				std::shared_ptr<TileMap::Tile> tile1 = GetTile(block->rect.xMax - 1, y);
				std::shared_ptr<TileMap::Tile> tile2 = GetTile(block->rect.xMax - 1, y + 1);
				if (Message::DungeonTileType::Floor == tile1->type && Message::DungeonTileType::Floor == tile2->type)
				{
					if (Message::DungeonTileType::Floor == GetTile(block->rect.xMax - 1 - 1, y)->type)
					{
						GetTile(block->rect.xMax -1, y)->type = Message::DungeonTileType::Invalid;
					}

					else //(Message::DungeonTileType::Floor == GetTile(block->rect.xMax -1 - 1, y + 1)->type)
					{
						GetTile(block->rect.xMax - 1, y + 1)->type = Message::DungeonTileType::Invalid;
					}
				}
			}

			// 상단 한쪽 벽에 문두개 연속으로 뚫는 부분 땜빵
			for (int x = block->rect.x; x < block->rect.xMax - 1; x++)
			{
				std::shared_ptr<TileMap::Tile> tile1 = GetTile(x, block->rect.y);
				std::shared_ptr<TileMap::Tile> tile2 = GetTile(x + 1, block->rect.y);
				if (Message::DungeonTileType::Floor == tile1->type && Message::DungeonTileType::Floor == tile2->type)
				{
					if (Message::DungeonTileType::Floor == GetTile(x, block->rect.y + 1)->type)
					{
						GetTile(x, block->rect.y)->type = Message::DungeonTileType::Invalid;
					}

					else // (Message::DungeonTileType::Floor == GetTile(x + 1, block->rect.y + 1)->type)
					{
						GetTile(x + 1, block->rect.y)->type = Message::DungeonTileType::Invalid;
					}
				}
			}

			// 하단 한쪽 벽에 문두개 연속으로 뚫는 부분 땜빵
			for (int x = block->rect.x; x < block->rect.xMax - 1; x++)
			{
				std::shared_ptr<TileMap::Tile> tile1 = GetTile(x, block->rect.yMax - 1);
				std::shared_ptr<TileMap::Tile> tile2 = GetTile(x + 1, block->rect.yMax - 1);
				if (Message::DungeonTileType::Floor == tile1->type && Message::DungeonTileType::Floor == tile2->type)
				{
					if (Message::DungeonTileType::Floor == GetTile(x, block->rect.yMax - 1 - 1)->type)
					{
						GetTile(x, block->rect.yMax - 1)->type = Message::DungeonTileType::Invalid;
					}
			
					else // (Message::DungeonTileType::Floor == GetTile(block->rect.xMax - 1 - 1, y + 1)->type)
					{
						GetTile(x + 1, block->rect.yMax - 1)->type = Message::DungeonTileType::Invalid;
					}
				}
			}
		}
		
		for (const auto& itr : blocks)
		{
			std::shared_ptr<Block> block = itr.second;
			if (Block::Type::Room != block->type)
			{
				continue;
			}
			for (int y = block->rect.y + 1; y < block->rect.yMax - 1; y++)
			{
				for (int x = block->rect.x + 1; x < block->rect.xMax - 1; x++)
				{
					std::shared_ptr<TileMap::Tile> tile = GetTile(x, y);
					tile->type = Message::DungeonTileType::Floor;
				}
			}
			for(int y=block->rect.y; y<block->rect.yMax; y++)
			{
				{
					std::shared_ptr<TileMap::Tile> tile = GetTile(block->rect.x, y);
					if(Message::DungeonTileType::Invalid == tile->type)
					{
						tile->type = Message::DungeonTileType::Wall;
					}
				}
				{
					std::shared_ptr<TileMap::Tile> tile = GetTile(block->rect.xMax-1, y);
					if (Message::DungeonTileType::Invalid == tile->type)
					{
						tile->type = Message::DungeonTileType::Wall;
					}
				}
			}

			for (int x = block->rect.x; x < block->rect.xMax; x++)
			{
				{
					std::shared_ptr<TileMap::Tile> tile = GetTile(x, block->rect.y);
					if (Message::DungeonTileType::Invalid == tile->type)
					{
						tile->type = Message::DungeonTileType::Wall;
					}
				}
				{
					std::shared_ptr<TileMap::Tile> tile = GetTile(x, block->rect.yMax - 1);
					if (Message::DungeonTileType::Invalid == tile->type)
					{
						tile->type = Message::DungeonTileType::Wall;
					}
				}
			}
		}
		
		for (const Vector2Int& path : paths)
		{
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x, path.y);
				if (nullptr != tile && Message::DungeonTileType::Floor != tile->type)
				{
					continue;
				}
			}
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x + 1, path.y);
				if(nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x + 1, path.y - 1);
				if (nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x, path.y - 1);
				if (nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}

			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x -1, path.y - 1);
				if (nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x - 1, path.y);
				if (nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x - 1, path.y + 1);
				if (nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x, path.y + 1);
				if (nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(path.x + 1, path.y + 1);
				if (nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}

			
		}

		for (int y = rect.y; y < rect.yMax; y++)
		{
			for (int x = rect.x; x < rect.xMax; x++)
			{
				std::shared_ptr<TileMap::Tile> tile = GetTile(x, y);
				if(Message::DungeonTileType::Invalid == tile->type)
				{
					std::cout << " ";
				}
				else if(Message::DungeonTileType::Floor == tile->type)
				{
					std::cout << ".";
				}
				else if (Message::DungeonTileType::Wall == tile->type)
				{
					std::cout << "#";
				}
				else
				{
					std::cout << "+";;
				}
				std::cout << " ";
			}
			std::cout << std::endl;
		}
		
		std::cout << "=====================" << std::endl;
	}

	void Data2::MoveToEmptySpace(std::shared_ptr<Block> block)
	{
		float angle = Gamnet::Random::Range(0.0f, 360.0f);
		float dx = cos(angle * 3.1415926535897f / 180.0f);
		float dy = sin(angle * 3.1415926535897f / 180.0f);
		float m = dy / dx;
		int increase = 0 == Gamnet::Random::Range(0, 1) ? 1 : -1;

		if (fabs(dy) > fabs(dx))
		{
			while (true)
			{
				if (false == OverlapWithExistBlocks(block))
				{
					return;
				}


				block->rect.y += increase;
				block->rect.x = (int)(block->rect.y / m);
			}
		}
		else
		{
			while (true)
			{
				if (false == OverlapWithExistBlocks(block))
				{
					return;
				}

			
				block->rect.x += increase;
				block->rect.y = (int)(block->rect.x * m);
			}
		}
	}
	bool Data2::OverlapWithExistBlocks(std::shared_ptr<Block> block)
	{
		for (const auto& itr : blocks)
		{
			std::shared_ptr<Block> exist = itr.second;
			if (true == exist->rect.Overlaps(block->rect))
			{
				return true;
			}
		}
		return false;
	}
	std::list<std::shared_ptr<Block>> Data2::FindNeighborBlocks(std::shared_ptr<Block> block)
	{
		RectInt rect = RectInt(block->rect.x - 1, block->rect.y - 1, block->rect.width + 2, block->rect.height + 2);
		
		std::list<std::shared_ptr<Block>> neighbors;

		for (const auto& itr: blocks)
		{
			std::shared_ptr<Block>  neighbor = itr.second;
			if (block == neighbor)
			{
				continue;
			}

			if (true == rect.Overlaps(neighbor->rect))
			{
				neighbors.push_back(neighbor);
			};
		}
		return neighbors;
	}
	
}}