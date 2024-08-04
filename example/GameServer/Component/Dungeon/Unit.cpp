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
	: Code("")
	, Index(0)
	, MaxHealth(0)
	, Attack(0)
	, Defense(0)
	, Speed(0)
	, Sight(0)
	, CriChance(0.0f)
	, CriRate(0.0f)
	, Monster(nullptr)
{
	META_MEMBER(Code);
	META_MEMBER(Index);
	META_MEMBER(MaxHealth);
	META_MEMBER(Attack);
	META_MEMBER(Defense);
	META_MEMBER(Speed);
	META_MEMBER(Sight);
	META_MEMBER(CriChance);
	META_MEMBER(CriRate);
	META_MEMBER(Monster);
}

void 
Meta::OnLoad()
{
	META_ASSERT("" != Code);
	META_ASSERT(0 != Index);
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
	Table::MetaReader<Meta> reader;
	reader.Read(path);
	for (auto& row : reader)
	{
		if (false == code_metas.insert(std::make_pair(row->Code, row)).second)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item id(", row->Code, ")");
		}
		if (false == index_metas.insert(std::make_pair(row->Index, row)).second)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item index(", row->Index, ")");
		}
	}
}
std::shared_ptr<Meta> Manager::FindMeta(const std::string& code)
{
	auto itr = code_metas.find(code);
	if (code_metas.end() == itr)
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
	if(nullptr != unit->meta->Monster)
	{
		unit->AddComponent<Monster::Data>(std::make_shared<Monster::Data>(unit->meta->Monster));
	}
	return unit;
}

std::shared_ptr<Unit::Data> CreatePlayer(std::shared_ptr<UserSession> session, int index)
{
	std::shared_ptr<Unit::Data> data = Gamnet::Singleton<Manager>::GetInstance().CreateInstance(index);
	data->team_index = 0;
	data->AddComponent<Player>();
	//session->AddComponent<Unit::Data>(data);
	return data;
}
}}

GAMNET_BIND_INIT_HANDLER(Component::Unit::Manager, Init);
