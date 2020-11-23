#ifndef _UNIT_H_
#define _UNIT_H_

#include <Gamnet/Library/Component.h>
#include "Unit/BehaviourTree.h"
#include "Vector2.h"

namespace Component { namespace Dungeon {
	class Data;
}}

class Unit : public std::enable_shared_from_this<Unit>, public Gamnet::Component
{
public :


	Unit(const std::shared_ptr<::Component::Dungeon::Data>& dungeon);

public :
	std::shared_ptr<::Component::Dungeon::Data> dungeon;
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

	int team_index;
	void SetPosition(const Vector2Int& position);
};

#endif
