#include "Unit.h"
#include <atomic>
#include "Dungeon.h"
#include <idl/MessageDungeon.h>
#include "../../UserSession.h"
#include "Unit/Player.h"
#include "Unit/Monster.h"

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

Data::Data()
	: dungeon(nullptr)
	, seq(++UNIT_SEQ)
	, max_health(0)
	, cur_health(0)
	, attack(0)
	, defense(0)
	, speed(0)
	, sight(10)
	, critical_chance(0.0f)
	, critical_rate(0.0f)
	, timer(Gamnet::Time::RepeatTimer::Create())
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
	InitMeta("../MetaData/Unit_Player.csv");
	InitMeta("../MetaData/Unit_Monster.csv");
}

void Manager::InitMeta(const std::string& path)
{
	MetaReader<Meta> reader;
	auto& rows = reader.Read(path);
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

std::shared_ptr<Unit::Data> Manager::CreateInstance(const std::string& id)
{
	std::shared_ptr<Meta> meta = FindMeta(id);
	if (nullptr == meta)
	{
		return nullptr;
	}

	return CreateInstance(meta);
}

std::shared_ptr<Unit::Data> Manager::CreateInstance(uint32_t index)
{
	std::shared_ptr<Meta> meta = FindMeta(index);
	if (nullptr == meta)
	{
		return nullptr;
	}

	return CreateInstance(meta);
}

std::shared_ptr<Unit::Data> Manager::CreateInstance(const std::shared_ptr<Meta>& meta)
{
	std::shared_ptr<Unit::Data> unit = std::make_shared<Unit::Data>();
	unit->meta = meta;
	if(nullptr != unit->meta->monster)
	{
		unit->AddComponent<Monster::Data>(std::make_shared<Monster::Data>(unit->meta->monster));
	}
	return unit;
}

std::shared_ptr<Unit::Data> CreatePlayer(std::shared_ptr<UserSession> session, const std::string& id)
{
	if(nullptr != session->GetComponent<Unit::Data>())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}
	std::shared_ptr<Unit::Data> data = Gamnet::Singleton<Manager>::GetInstance().CreateInstance(id);
	data->team_index = 0;
	data->AddComponent<Player>();
	session->AddComponent<Unit::Data>(data);
	return data;
}
}}

GAMNET_BIND_INIT_HANDLER(Component::Unit::Manager, Init);
