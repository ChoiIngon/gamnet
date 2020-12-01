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

bool Meta::IsVisible(std::shared_ptr<Unit::Data>& self, const Vector2Int& target)
{
	auto dungeon = self->dungeon;
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

void Meta::Update(std::shared_ptr<Unit::Data> data)
{
	behaviour->Run(data);
}

void Meta::OnBehaviourPath(std::shared_ptr<BehaviourTree<std::shared_ptr<Unit::Data>>>& behaviour, const std::string& value)
{
	behaviour = std::make_shared<BehaviourTree<std::shared_ptr<Unit::Data>>>();
	BehaviourTree<std::shared_ptr<Unit::Data>>::Meta meta;
	meta.BindAction<Action::SearchTarget>("SearchTarget");
	meta.BindAction<Action::FindPathToTarget>("FindPathToTarget");
	meta.BindAction<Action::MoveToTarget>("MoveToTarget");
	behaviour->root = meta.ReadXml("../MetaData/" + value);
}

Data::Data(std::shared_ptr<Meta> meta)
	: meta(meta)
	, target(nullptr)
{
}

Data::~Data()
{
}
}}
