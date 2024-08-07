#include "Dungeon.h"
#include "Unit.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include <iostream>
#include "AStarPathFinder.h"
#include "../../UserSession.h"
#include "../UserData/Suit.h"
#include "../UserData/Item.h"

namespace Component { namespace Dungeon {

	static std::atomic<uint64_t> DUNGEON_SEQ;
	Meta::Room::Room()
		: Count(0)
		, MinWidth(0)
		, MaxWidth(0)
		, MinHeight(0)
		, MaxHeight(0)
	{
		META_MEMBER(Count);
		META_MEMBER(MinWidth);
		META_MEMBER(MaxWidth);
		META_MEMBER(MinHeight);
		META_MEMBER(MaxHeight);
	}

	Meta::Monster::Monster()
		: Code("")
		, Level(0)
		, Count(0)
	{
		META_MEMBER(Code);
		META_MEMBER(Level);
		META_MEMBER(Count);
	}

	Meta::Meta()
		: Code("")
		, Index(0)
		, Level(0)
	{
		META_MEMBER(Code);
		META_MEMBER(Index);
		META_MEMBER(Level);
		META_MEMBER(Room);
		META_MEMBER(Monster);
	}
	void Meta::OnLoad()
	{
		Room.Count = std::max(Room.Count, 1);
		Room.MinWidth = std::max(Room.MinWidth, 3);
		Room.MaxWidth = std::max(Room.MaxWidth, 3);
		Room.MinHeight = std::max(Room.MinHeight, 3);
		Room.MaxHeight = std::max(Room.MinHeight, 3);
	}

	Data::Block::Block(int id)
		: id(id)
		, type(Type::Invalid)
	{
	}
	
	std::shared_ptr<Data> Meta::CreateInstance()
	{
		std::shared_ptr<Data> data = std::make_shared<Data>(*this);
		data->Init();
		return data;
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
		seq = ++DUNGEON_SEQ;
		blocks.clear();

		RectInt rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = 0;
		rect.height = 0;
		
		// 블럭을 생성하여 랜덤하게 여기저기 뿌림
		for(int i=0; i<meta.Room.Count * Data::ROOM_COUNT_MULTIPLE; i++)
		{
			std::shared_ptr<Block> block = std::make_shared<Block>(i+1);
			block->rect.x = 0;
			block->rect.y = 0;

			if (0 == i % Data::ROOM_COUNT_MULTIPLE)
			{
				block->type = Block::Type::Room;
				block->rect.width = Gamnet::Random::Range(meta.Room.MinWidth, meta.Room.MaxWidth);
				block->rect.height = Gamnet::Random::Range(meta.Room.MinHeight, meta.Room.MaxHeight);
			}
			else
			{
				block->type = Block::Type::Corridor;
				block->rect.width = Gamnet::Random::Range(meta.Room.MinWidth/2, meta.Room.MaxWidth/2);
				block->rect.height = Gamnet::Random::Range(meta.Room.MinHeight/2, meta.Room.MaxHeight/2);
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
	void Data::MoveToEmptySpace(std::shared_ptr<Data::Block> block)
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
	bool Data::OverlapWithExistBlocks(std::shared_ptr<Data::Block> block)
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
	std::list<std::shared_ptr<Data::Block>> Data::FindNeighborBlocks(std::shared_ptr<Data::Block> block)
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

	size_t Data::GetPlayerCount() const
	{
		return sessions.size();
	}

	size_t Data::Enter(std::shared_ptr<UserSession> session)
	{
		/*
		session->strand = this->strand;
		std::shared_ptr<Unit::Data> player = session->GetComponent<Component::Unit::Data>();
		player->dungeon = this;
		player->position = start->rect.Center();

		if(0 < sessions.size())
		{
			Message::Dungeon::MsgSvrCli_CreatePlayer_Ntf ntf;
			ntf.unit_seq = player->seq;

			std::shared_ptr<Component::Suit> suit = session->GetComponent<Component::Suit>();
			for (int part = (int)Message::EquipItemPartType::Invalid + 1; part < (int)Message::EquipItemPartType::Max; part++)
			{
				std::shared_ptr<Item::Data> item = suit->Find((Message::EquipItemPartType)part);
				if (nullptr == item)
				{
					continue;
				}

				ntf.equip_items.push_back(item->meta->Index);
			}

			ntf.position = Message::Vector2Int(player->position.x, player->position.y);
			SendMsg(ntf);
		}
		
		players.insert(std::make_pair(player->seq, player));
		sessions.insert(std::make_pair(session->session_key, session));
		return sessions.size();
		*/
		return 0;
	}
	void Data::Leave(std::shared_ptr<UserSession> session)
	{
		/*
		session->strand = Gamnet::Network::Session::CreateStrand();
		std::shared_ptr<Unit::Data> player = session->GetComponent<Component::Unit::Data>();
		player->dungeon = nullptr;
		players.erase(player->seq);
		sessions.erase(session->session_key);

		if(0 < sessions.size())
		{
			Message::Dungeon::MsgSvrCli_DestroyUnit_Ntf ntf;
			ntf.unit_seq = player->seq;
			SendMsg(ntf);
		}
		*/
	}
	void Manager::Init()
	{
		Table::MetaReader<Meta> reader;
		reader.Read("../MetaData/Dungeon.csv");
		for (auto& row : reader)
		{
			if (false == id_metas.insert(std::make_pair(row->Code, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item id(", row->Code, ")");
			}
			if (false == index_metas.insert(std::make_pair(row->Index, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item index(", row->Index, ")");
			}
			wait_queues.insert(std::make_pair(row->Index, std::make_shared<WaitQueue>()));
		}
	}

	GAMNET_BIND_INIT_HANDLER(Dungeon::Manager, Init);

	std::shared_ptr<Meta> Manager::FindMeta(const std::string& id)
	{
		auto itr = id_metas.find(id);
		if (id_metas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	std::shared_ptr<Meta> Dungeon::Manager::FindMeta(uint32_t index)
	{
		auto itr = index_metas.find(index);
		if (index_metas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	std::shared_ptr<Data> Dungeon::Manager::Enter(uint32_t index, std::shared_ptr<UserSession> session)
	{
		std::shared_ptr<Data> data = nullptr;

		auto itr = wait_queues.find(index);
		if(itr == wait_queues.end())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}

		auto& queue = itr->second;
		data = queue->Pop();

		if(nullptr != data)
		{
			size_t count = data->Enter(session);
			if(count < WaitQueue::MAX_PLAER)
			{
				queue->Push(data);
			}
			return data;
		}

		std::shared_ptr<Meta> meta = FindMeta(index);
		if(nullptr == meta)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}

		data = meta->CreateInstance();
		size_t count = data->Enter(session);
		if (count < WaitQueue::MAX_PLAER)
		{
			queue->Push(data);
		}
		return data;
	}

	std::shared_ptr<Data> Dungeon::Manager::WaitQueue::Pop()
	{
		std::mutex lock;
		for(int count=MAX_PLAER; count>=0; count--)
		{
			std::list<std::shared_ptr<Data>>& queue = queue_by_player_count.at(count);
			
			if(0 < queue.size())
			{
				std::shared_ptr<Data> data = queue.front();
				queue.pop_front();
				return data;
			}
		}
		return nullptr;
	}

	void Dungeon::Manager::WaitQueue::Push(std::shared_ptr<Data> data)
	{
		std::mutex lock;
		size_t playerCount = data->GetPlayerCount();
		std::list<std::shared_ptr<Data>>& queue = queue_by_player_count.at(playerCount);
		queue.push_back(data);
	}

	std::shared_ptr<Data> Enter(std::shared_ptr<UserSession> session, uint32_t index)
	{
		std::shared_ptr<Data> data = Gamnet::Singleton<Dungeon::Manager>::GetInstance().Enter(index, session);
		//session->AddComponent<Component::Dungeon::Data>(data);
		return data;
	}
}}