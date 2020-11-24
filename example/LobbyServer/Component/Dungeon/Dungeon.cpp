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
	{
		META_MEMBER(count);
		META_MEMBER(min_width);
		META_MEMBER(max_width);
		META_MEMBER(min_height);
		META_MEMBER(max_height);
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
		std::shared_ptr<Data> data = std::make_shared<Data>(*this);
		data->Init();
		return data;
	}

	void Meta::OnLoad()
	{
		room.count = std::max(room.count, 1);
		room.min_width = std::max(room.min_width, 3);
		room.max_width = std::max(room.max_width, 3);
		room.min_height = std::max(room.min_height, 3);
		room.max_height = std::max(room.min_height, 3);
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

	const Vector2Int Data::EIGHT_DIRECTION_OFFSET[8] = {
		Vector2Int( 1, 0),
		Vector2Int( 1,-1),
		Vector2Int( 0,-1),
		Vector2Int(-1,-1),
		Vector2Int(-1, 0),
		Vector2Int(-1, 1),
		Vector2Int( 0, 1),
		Vector2Int( 1, 1)
	};

	Data::Data(const Meta& meta) 
		: meta(meta)
		, strand(Gamnet::Network::Session::CreateStrand())
		, start(nullptr)
		, end(nullptr)
	{
	}

	void Data::Init()
	{
		blocks.clear();

		RectInt rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = 0;
		rect.height = 0;
		
		// 블럭을 생성하여 랜덤하게 여기저기 뿌림
		for(int i=0; i<meta.room.count * Data::ROOM_COUNT_MULTIPLE; i++)
		{
			std::shared_ptr<Block> block = std::make_shared<Block>(i+1);
			block->rect.x = 0;
			block->rect.y = 0;

			if (0 == i % Data::ROOM_COUNT_MULTIPLE)
			{
				block->type = Block::Type::Room;
				block->rect.width = Gamnet::Random::Range(meta.room.min_width, meta.room.max_width);
				block->rect.height = Gamnet::Random::Range(meta.room.min_height, meta.room.max_height);
			}
			else
			{
				block->type = Block::Type::Corridor;
				block->rect.width = Gamnet::Random::Range(meta.room.min_width/2, meta.room.max_width/2);
				block->rect.height = Gamnet::Random::Range(meta.room.min_height/2, meta.room.max_height/2);
			}
			MoveToEmptySpace(block);

			int xMax = rect.xMax;
			int yMax = rect.yMax;
			rect.x = std::min(rect.x, block->rect.x);
			rect.y = std::min(rect.y, block->rect.y);
			rect.xMax = std::max(xMax, (int)block->rect.xMax);
			rect.yMax = std::max(yMax, (int)block->rect.yMax);
			blocks.insert(std::make_pair(block->id, block));
		}

		// 뿌려진 블록들을 모두 포함하는 Rect 생성
		for (const auto& itr : blocks)
		{
			std::shared_ptr<Block> block = itr.second;
			block->rect.x -= rect.x;
			block->rect.y -= rect.y;
		}

		rect.x = 0;
		rect.y = 0;
		
		// TileMap을 최대 rect size로 맞춤
		SetMapSize(rect.width, rect.height);
		
		// 각 방을 연결하는 패스 설정
		std::list<Vector2Int> paths = BuildPath();
		
		// 패스에 마루 깔기
		for (const Vector2Int& path : paths)
		{
			std::shared_ptr<Tile> tile = GetTile(path.x, path.y);
			tile->type = Message::DungeonTileType::Floor;
		}
		
		for (const auto& itr : blocks)
		{
			std::shared_ptr<Block> block = itr.second;
			if(Block::Type::Room != block->type)
			{
				continue;
			}
			
			for (int y = block->rect.y; y < block->rect.yMax - 1; y++)
			{
				{
					// 좌측 한쪽 벽에 문두개 연속으로 뚫는 부분 땜빵
					std::shared_ptr<Tile> tile = GetTile(block->rect.x, y);
					if (Message::DungeonTileType::Floor == tile->type)
					{
						if (Message::DungeonTileType::Floor != GetTile(block->rect.x - 1, y)->type)
						{
							GetTile(block->rect.x, y)->type = Message::DungeonTileType::Invalid;

						}
					}
				}
				{
					// 우측 한쪽 벽에 문두개 연속으로 뚫는 부분 땜빵
					std::shared_ptr<Tile> tile = GetTile(block->rect.xMax - 1, y);
					if (Message::DungeonTileType::Floor == tile->type)
					{
						if (Message::DungeonTileType::Floor != GetTile(block->rect.xMax, y)->type)
						{
							GetTile(block->rect.xMax - 1, y)->type = Message::DungeonTileType::Invalid;
						}
					}
				}
			}
			
			for (int x = block->rect.x; x < block->rect.xMax - 1; x++)
			{
				{
					// 하단 한쪽 벽에 문두개 연속으로 뚫는 부분 땜빵
					std::shared_ptr<Tile> tile = GetTile(x, block->rect.y);
					if (Message::DungeonTileType::Floor == tile->type)
					{
						if (Message::DungeonTileType::Floor != GetTile(x, block->rect.y - 1)->type)
						{
							GetTile(x, block->rect.y)->type = Message::DungeonTileType::Invalid;
						}
					}
				}
				{
					// 상단 한쪽 벽에 문두개 연속으로 뚫는 부분 땜빵
					std::shared_ptr<Tile> tile = GetTile(x, block->rect.yMax - 1);
					if (Message::DungeonTileType::Floor == tile->type)
					{
						if (Message::DungeonTileType::Floor != GetTile(x, block->rect.yMax)->type)
						{
							GetTile(x, block->rect.yMax - 1)->type = Message::DungeonTileType::Invalid;
						}
					}
				}
			}
		}
		
		// 방에 벽 세우고 바닥 채우기
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
					std::shared_ptr<Tile> tile = GetTile(x, y);
					tile->type = Message::DungeonTileType::Floor;
				}
			}

			for (int y=block->rect.y; y<block->rect.yMax; y++)
			{
				for(int x = block->rect.x; x < block->rect.xMax; x += block->rect.width - 1)
				{
					std::shared_ptr<Tile> tile = GetTile(x, y);
					if(Message::DungeonTileType::Invalid == tile->type)
					{
						tile->type = Message::DungeonTileType::Wall;
					}
				}
			}

			for (int x = block->rect.x; x < block->rect.xMax; x++)
			{
				for (int y = block->rect.y; y < block->rect.yMax; y += block->rect.height - 1)
				{
					std::shared_ptr<Tile> tile = GetTile(x, y);
					if (Message::DungeonTileType::Invalid == tile->type)
					{
						tile->type = Message::DungeonTileType::Wall;
					}
				}
			}
		}
		
		// 복도 벽 세우기
		for (const Vector2Int& path : paths)
		{
			{
				std::shared_ptr<Tile> tile = GetTile(path.x, path.y);
				if (nullptr != tile && Message::DungeonTileType::Floor != tile->type)
				{
					continue;
				}
			}
			
			for(const Vector2Int offset : EIGHT_DIRECTION_OFFSET)
			{
				std::shared_ptr<Tile> tile = GetTile(path.x + offset.x, path.y + offset.y);
				if(nullptr != tile && Message::DungeonTileType::Invalid == tile->type)
				{
					tile->type = Message::DungeonTileType::Wall;
				}
			}
		}
	}
	void Data::MoveToEmptySpace(std::shared_ptr<Block> block)
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
	bool Data::OverlapWithExistBlocks(std::shared_ptr<Block> block)
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
	std::list<std::shared_ptr<Block>> Data::FindNeighborBlocks(std::shared_ptr<Block> block)
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
	std::list<Vector2Int> Data::BuildPath()
	{
		std::vector<std::shared_ptr<Block>> shuffledBlocks;
		for(const auto& itr : blocks)
		{
			std::shared_ptr<Block> block = itr.second;
			shuffledBlocks.push_back(block);
		}
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(shuffledBlocks.begin(), shuffledBlocks.end(), g);
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

		for(int i=0; i<edges.size(); i++)
		{
			if(Block::Type::Room == edges[i]->block->type)
			{
				this->start = edges[i]->block;
				break;
			}
		}
		for (int i = edges.size()-1; i >= 0; i--)
		{
			if (Block::Type::Room == edges[i]->block->type)
			{
				this->end = edges[i]->block;
				break;
			}
		}
		//////////////////// AStarPath finding //////////////////////////////////////

		for (const auto& itr : blocks)	// 벽 침범을 방지하기 위해 방으로 설정된 블록 각 기둥에 벽을 미리 박음. 그러면 패스 파인딩에서 피해감
		{
			std::shared_ptr<Block> block = itr.second;
			if (Block::Type::Room == block->type)
			{
				GetTile(block->rect.x, block->rect.y)->type = Message::DungeonTileType::Wall;
				GetTile(block->rect.xMax - 1, block->rect.y)->type = Message::DungeonTileType::Wall;
				GetTile(block->rect.x, block->rect.yMax - 1)->type = Message::DungeonTileType::Wall;
				GetTile(block->rect.xMax - 1, block->rect.yMax - 1)->type = Message::DungeonTileType::Wall;
			}
		}

		std::list<Vector2Int> paths;
		for (std::shared_ptr<Edge> edge : edges)
		{
			if (nullptr == edge->parent)
			{
				continue;
			}

			if (Block::Type::Room != edge->block->type)
			{
				continue;
			}

			std::shared_ptr<Edge> itr = edge;
			while (nullptr != itr->parent)
			{
				AStarPathFinder pathFinder(*this, itr->block->rect.Center(), itr->parent->block->rect.Center());
				paths.insert(paths.end(), pathFinder.path.begin(), pathFinder.path.end());
				itr = itr->parent;
			}
		}
		return paths;
	}
}}