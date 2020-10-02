#ifndef _UNIT_H_
#define _UNIT_H_

#include <Gamnet/Library/Component.h>
#include "Unit/BehaviourTree.h"
#include "Vector2.h"

namespace Component { 
	class Dungeon;
}

class Unit : public std::enable_shared_from_this<Unit>
{
public :
	typedef Gamnet::Component Attribute;

	Unit();

public :
	const uint64_t seq;
	int max_health;
	int cur_health;
	int attack;
	int defense;
	int speed;
	int	sight;
	float critical_chance;
	float critical_rate;
	Vector2Int	position;

	std::shared_ptr<Attribute>	attributes;

	std::shared_ptr<Component::Dungeon> dungeon;

	void SetPosition(const Vector2Int& position);
};

#endif
