#include "Unit.h"
#include <atomic>
#include "Dungeon.h"

static std::atomic<uint64_t> UNIT_SEQ = 0;

Unit::Unit()
	: seq(++UNIT_SEQ)
	, max_health(0)
	, cur_health(0)
	, attack(0)
	, defense(0)
	, speed(0)
	, sight(10)
	, critical_chance(0.0f)
	, critical_rate(0.0f)
	, attributes(std::make_shared<Attribute>())
{
}

void Unit::SetPosition(const Vector2Int& position)
{
	if(nullptr == dungeon)
	{
		return;
	}

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