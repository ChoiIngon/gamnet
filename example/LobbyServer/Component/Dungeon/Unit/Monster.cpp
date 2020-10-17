#include "Monster.h"
#include "MonsterAction.h"
#include "../Unit.h"
#include "../Dungeon.h"
#include "../BresenhamLine2D.h"
#include <Gamnet/Library/Singleton.h>
#include <idl/MessageCommon.h>


namespace Component { namespace Monster {
Meta::Meta()
	: id("")
	, index(0)
	, name("")
	, behaviour(nullptr)
{
	META_MEMBER(id);
	META_MEMBER(index);
	META_MEMBER(name);
	META_CUSTOM(behaviour, Meta::OnBehaviourPath);
}

bool Meta::IsVisible(std::shared_ptr<Unit>& self, const Vector2Int& target)
{
	auto dungeon = self->attributes->GetComponent<Component::Dungeon>();
	if(nullptr == dungeon)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}
	
	if(self->sight < Vector2Int::Distance(self->position, target))
	{
		return false;
	}

	BresenhamLine2D line(self->position, target);
	for(const Vector2Int& point : line.GetEnumerator())
	{
		auto tile = dungeon->GetTile(point.x, point.y);
		if(Tile::Type::Wall == tile->type)
		{
			return false;
		}
	}
	return true;
}

void Meta::Update(std::shared_ptr<Unit> data)
{
	behaviour->Run(data);
}

void Meta::OnBehaviourPath(std::shared_ptr<BehaviourTree<std::shared_ptr<Unit>>>& behaviour, const std::string& value)
{
	behaviour = std::make_shared<BehaviourTree<std::shared_ptr<Unit>>>();
	BehaviourTree<std::shared_ptr<Unit>>::Meta meta;
	meta.BindAction<Action::SearchTarget>("SearchTarget");
	meta.BindAction<Action::FindPathToTarget>("FindPathToTarget");
	meta.BindAction<Action::MoveToTarget>("MoveToTarget");
	behaviour->root = meta.ReadXml("../MetaData/" + value);
}

Data::Data()
	: meta(nullptr)
	, target(nullptr)
{
}

Data::~Data()
{
}

void Manager::Init()
{
	MetaReader<Meta> reader;
	auto& rows = reader.Read("../MetaData/Monster.csv");
	for (auto& row : rows)
	{
		if(false == index_metas.insert(std::make_pair(row->index, row)).second)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate monster index(", row->index, ")");
		}
		if(false == id_metas.insert(std::make_pair(row->id, row)).second)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate monster id(", row->id, ")");
		}
	}
}

std::shared_ptr<Meta> Manager::FindMeta(const std::string& id)
{
	auto itr = id_metas.find(id);
	if(id_metas.end() == itr)
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

std::shared_ptr<Unit> Manager::CreateInstance(const std::string& id)
{
	std::shared_ptr<Meta> meta = FindMeta(id);
	if(nullptr == meta)
	{
		return nullptr;
	}

	return CreateInstance(meta);
}

std::shared_ptr<Unit> Manager::CreateInstance(uint32_t index)
{
	std::shared_ptr<Meta> meta = FindMeta(index);
	if (nullptr == meta)
	{
		return nullptr;
	}

	return CreateInstance(meta);
}

std::shared_ptr<Unit> Manager::CreateInstance(const std::shared_ptr<Meta>& meta)
{
	std::shared_ptr<Data> data = std::make_shared<Data>();
	data->meta = meta;

	std::shared_ptr<Unit> unit = std::make_shared<Unit>();
	unit->attributes->AddComponent<Data>(data);
	return unit;
}

}}
GAMNET_BIND_INIT_HANDLER(Component::Monster::Manager, Init);