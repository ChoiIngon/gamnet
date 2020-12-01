#include "Unit.h"
#include <atomic>
#include "Dungeon.h"
#include <idl/MessageDungeon.h>

namespace Component { namespace Unit {
static std::atomic<uint64_t> UNIT_SEQ;

Meta::Meta()
	: id("")
	, index(0)
	, max_health(0)
	, attack(0)
	, defense(0)
	, speed(0)
	, sight(0)
	, critical_chance(0.0f)
	, critical_rate(0.0f)
{
	META_MEMBER(id);
	META_MEMBER(index);
	META_MEMBER(monster);
}

Data::Data(const std::shared_ptr<::Component::Dungeon::Data>& dungeon)
	: dungeon(dungeon)
	, seq(++UNIT_SEQ)
	, max_health(0)
	, cur_health(0)
	, attack(0)
	, defense(0)
	, speed(0)
	, sight(10)
	, critical_chance(0.0f)
	, critical_rate(0.0f)
	, timer(std::make_shared<Gamnet::Time::Timer>())
{
}

void Data::SetPosition(const Vector2Int& position)
{
	std::shared_ptr<Tile> nextTile = dungeon->GetTile(position);
	if (nullptr == nextTile)
	{
		return;
	}

	if(nullptr != nextTile->unit && shared_from_this() != nextTile->unit)
	{
		return;
	}
	
	std::shared_ptr<Tile> curTile = dungeon->GetTile(this->position);
	if(nullptr != curTile)
	{
		curTile->unit = nullptr;
	}

	this->position = position;
	nextTile->unit = shared_from_this();
}

void Manager::Init()
{
	MetaReader<Meta> reader;
	{
		std::shared_ptr<Meta> player = std::make_shared<Meta>();
		player->id = "Player";
		player->index = 1;
		index_metas.insert(std::make_pair(player->index, player));
		id_metas.insert(std::make_pair(player->id, player));
	}
	{
		auto& rows = reader.Read("../MetaData/Unit_Monster.csv");
		for (auto& row : rows)
		{
			if (false == index_metas.insert(std::make_pair(row->index, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate monster index(", row->index, ")");
			}
			if (false == id_metas.insert(std::make_pair(row->id, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate monster id(", row->id, ")");
			}
		}
	}
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

std::shared_ptr<Unit::Data> Manager::CreateInstance(const std::string& id, std::shared_ptr<Component::Dungeon::Data> dungeon)
{
	std::shared_ptr<Meta> meta = FindMeta(id);
	if (nullptr == meta)
	{
		return nullptr;
	}

	return CreateInstance(meta, dungeon);
}

std::shared_ptr<Unit::Data> Manager::CreateInstance(uint32_t index, std::shared_ptr<Component::Dungeon::Data> dungeon)
{
	std::shared_ptr<Meta> meta = FindMeta(index);
	if (nullptr == meta)
	{
		return nullptr;
	}

	return CreateInstance(meta, dungeon);
}

std::shared_ptr<Unit::Data> Manager::CreateInstance(const std::shared_ptr<Meta>& meta, std::shared_ptr<Component::Dungeon::Data> dungeon)
{
	std::shared_ptr<Unit::Data> unit = std::make_shared<Unit::Data>(dungeon);
	unit->meta = meta;
	if(nullptr != unit->meta->monster)
	{
		unit->AddComponent<Monster::Data>(std::make_shared<Monster::Data>(unit->meta->monster));
	}
	return unit;
}

}}

GAMNET_BIND_INIT_HANDLER(Component::Unit::Manager, Init);
