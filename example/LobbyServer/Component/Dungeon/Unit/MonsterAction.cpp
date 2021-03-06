#include "MonsterAction.h"
#include "../Unit.h"
#include "../BresenhamLine2D.h"
#include "Monster.h"
#include "Player.h"

namespace Component { namespace Monster { namespace Action {

	bool SearchTarget::Run(std::shared_ptr<Unit::Data> self)
	{
		/*
		std::shared_ptr<Monster::Data> monster = self->GetComponent<Monster::Data>();
		if(nullptr != monster->target)
		{
			std::shared_ptr<Unit::Data> target = monster->target;
			float distance = Vector2Int::Distance(self->position, target->position);
			if(distance > 2 * self->sight)
			{
				monster->target = nullptr;
				return false;
			}
			return true;
		}

		std::shared_ptr<Unit::Data> player = self->dungeon->player;
		float distance = Vector2Int::Distance(self->position, player->position);
	
		if(distance > self->sight)
		{
			return false;
		}

		BresenhamLine2D line(self->position, player->position);
		for(const Vector2Int& point : line.GetEnumerator())
		{
			auto tile = self->dungeon->GetTile(point.x, point.y);
			if(Tile::Type::Wall == tile->type)
			{
				return false;
			}
		}

		monster->target = player;
		*/
		return true;
	}
	
	bool FindPathToTarget::Run(std::shared_ptr<Unit::Data> self)
	{
		std::shared_ptr<Monster::Data> monster = self->GetComponent<Monster::Data>();
		std::shared_ptr<Unit::Data> target = monster->target;
		if (nullptr == target)
		{
			return false;
		}

		if(0 < monster->path.size())
		{
			return true;
		}

		//std::shared_ptr<Component::Dungeon::Data> dungeon = self->dungeon;
		//AStarPathFinder pathFinder(dungeon, self->position, target->position);
		//monster->path = pathFinder.path;
		
		return true;
	}
	
	bool MoveToTarget::Run(std::shared_ptr<Unit::Data> self)
	{
		std::shared_ptr<Monster::Data> monster = self->GetComponent<Monster::Data>();
		if(0 == monster->path.size())
		{
			return false;
		}

		self->SetPosition(*(monster->path.begin()));
	 	monster->path.pop_front();
		return true;
	}
}}}